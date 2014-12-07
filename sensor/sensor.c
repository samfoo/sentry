#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <signal.h>

#include <iostream>
#include <algorithm>

#include "libfreenect.h"
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/video/background_segm.hpp"
#include "opencv2/video/tracking.hpp"

#include "stream.h"

#define WIDTH 640
#define HEIGHT 480

#define MIN_DEPTH 400
#define MAX_DEPTH 6000

#define HOR_FIELD 57
#define VER_FIELD 43

#define MIN_DENSITY_BEFORE_RETRACK 5

bool die = false;
uint8_t kinect_rgb_buffer[WIDTH*HEIGHT*3];

bool started = false;

pthread_mutex_t render_buffer_mutex = PTHREAD_MUTEX_INITIALIZER;

cv::Mat movement_buffer;

cv::Point cursor_pos = cv::Point(0, 0);

bool tracking_object = false;
cv::Mat image_prev;
cv::Mat image_current;

uint16_t distance_to_features;
std::vector<cv::Point2f> features_current;
cv::Point2f features_center;

cv::Mat bgr_buffer;

uint16_t depth_values[WIDTH*HEIGHT];
cv::Mat depth_buffer;

cv::BackgroundSubtractorMOG2 background_subtractor;

cv::Mat foreground_mask;

pthread_t kinect_thread;

freenect_context *context;
freenect_device *device;

void depth_captured(freenect_device *dev, void *v, uint32_t timestamp) {
    uint16_t *depths = (uint16_t*)v;
    uint8_t *p = depth_buffer.ptr<uint8_t>(0);

    for (int i=0; i < WIDTH*HEIGHT; i++) {
        depth_values[i] = depths[i];
        if (depths[i] < MIN_DEPTH || depths[i] > MAX_DEPTH) {
            p[i] = 0xff;
        }
        else {
            p[i] = (uint8_t)(0xff * (float)(depths[i] - MIN_DEPTH) / (float)(MAX_DEPTH - MIN_DEPTH));
        }
    }
}

bool area_comparitor(const std::vector<cv::Point> &a, const std::vector<cv::Point> &b) {
    int a_area = cv::contourArea(a);
    int b_area = cv::contourArea(b);

    return a_area > b_area;
}

uint16_t distance_at_point(cv::Point2f &point) {
    return depth_values[(int)point.y * WIDTH + (int)point.x];
}

int tracking_density() {
    if (features_current.size() > 0) {
        cv::Point first = features_current[0];
        int max_x = first.x;
        int max_y = first.y;
        int min_x = first.x;
        int min_y = first.y;

        for (int i=0; i < features_current.size(); i++) {
            cv::Point p = features_current[i];
            max_x = std::max(max_x, p.x);
            max_y = std::min(min_y, p.y);
            min_x = std::max(max_x, p.x);
            min_y = std::min(min_y, p.y);
        }

        int area = (max_x - min_x) * (max_y - min_y);

        return features_current.size() / area;
    } else {
        return 0;
    }
}

void draw_features_center(cv::Mat &buffer) {
    if (features_current.size() > 0 &&
            (features_center.x > 0 || features_center.y > 0)) {
        circle(buffer, features_center, 5, cv::Scalar(255, 0, 255), CV_FILLED);
    }
}

void draw_current_points(cv::Mat &buffer) {
    for (int i=0; i < features_current.size(); i++) {
        cv::Point p = features_current[i];
        circle(buffer, p, 1, cv::Scalar(0, 255, 0), 2);
    }
}

void draw_cursor_details(cv::Mat &buffer) {
    std::ostringstream out;
    uint16_t distance = depth_values[cursor_pos.y * WIDTH + cursor_pos.x];
    out << distance << "mm";
    cv::putText(buffer, out.str(), cursor_pos,
            CV_FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 0, 255), 2);
}

void draw_distance_to_features(cv::Mat &buffer) {
    if (features_current.size() > 0) {
        std::ostringstream out;
        out << "target is " << distance_to_features << "mm away";

        cv::putText(buffer, out.str(), cv::Point(10,20),
                CV_FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 0, 255), 2);
    }
}

void firing_solution() {
    float normalized_x = 1 - features_center.x / WIDTH;
    float normalized_y = features_center.y / HEIGHT;

    float aim_pan = normalized_x * HOR_FIELD;
    float aim_tilt = normalized_y * VER_FIELD;

    send_position((int)aim_pan, (int)aim_tilt);
}

void estimate_distance_to_features() {
    const int clusters = 1;

    if (features_current.size() > clusters) {
        cv::Mat labels;
        cv::Mat centers(clusters, 1, CV_32FC2);

        kmeans(cv::Mat(features_current), clusters, labels,
                cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 10, 1.0),
                3, cv::KMEANS_PP_CENTERS, centers);

        std::vector<cv::Point2f> _centers = cv::Mat_ <cv::Point2f>(centers);

        features_center = _centers[0];
        distance_to_features = distance_at_point(features_center);
    }
}

void step_tracking() {
    if (features_current.size() < 40) {
        tracking_object = false;
    }

    if (tracking_object &&
            tracking_density() < MIN_DENSITY_BEFORE_RETRACK) {
        movement_buffer.setTo(cv::Scalar::all(0));
        cv::circle(movement_buffer, features_center, 50, cv::Scalar(255, 255, 255), CV_FILLED);

        tracking_object = false;
    }

    if (!tracking_object) {
        cv::goodFeaturesToTrack(
                image_current,
                features_current,
                1000, 0.02, 4,
                movement_buffer);

        tracking_object = true;
        return;
    }

    std::vector<cv::Point2f> features_prev = features_current;
    std::vector<uchar> features_status;
    std::vector<float> features_error;

    if (features_current.size() > 0) {
        std::vector<cv::Point2f> next = std::vector<cv::Point2f>();
        features_current = std::vector<cv::Point2f>();

        calcOpticalFlowPyrLK(image_prev, image_current,
                features_prev, next,
                features_status, features_error);

        for (int i=0; i < features_status.size(); i++) {
            if (features_status[i]) {
                features_current.push_back(next[i]);
            }
        }
    }
}

void mouse_events(int event, int x, int y, int flags, void* userdata) {
    if (event == cv::EVENT_LBUTTONDOWN) {
        cv::Point center = cv::Point(x, y);

        pthread_mutex_lock(&render_buffer_mutex);

        movement_buffer.setTo(cv::Scalar::all(0));
        cv::circle(movement_buffer, center, 50, cv::Scalar(255, 255, 255), CV_FILLED);

        tracking_object = false;
        step_tracking();

        pthread_mutex_unlock(&render_buffer_mutex);
    }
    else if (event == cv::EVENT_RBUTTONDOWN) {
        pthread_mutex_lock(&render_buffer_mutex);

        movement_buffer.setTo(cv::Scalar::all(0));
        tracking_object = false;
        features_current.clear();

        pthread_mutex_unlock(&render_buffer_mutex);
    }
    else if (event == cv::EVENT_MOUSEMOVE) {
        cursor_pos = cv::Point(x, y);
    }
}

void detect_movement() {
    background_subtractor(bgr_buffer, foreground_mask);

    cv::erode(foreground_mask, foreground_mask, cv::Mat());
    cv::dilate(foreground_mask, foreground_mask, cv::Mat());

    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(foreground_mask, contours,
            CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

    std::sort(contours.begin(), contours.end(), area_comparitor);

    std::vector<std::vector<cv::Point> > best(contours.begin(), contours.begin() + fmin(1, contours.size()));

    if (best.size() > 0) {
        std::vector<cv::Point> best_guess = best[0];
        int area = cv::contourArea(best_guess);

        movement_buffer.setTo(cv::Scalar::all(0));

        if (area > 1000 && area < 5000) {
            std::vector<cv::Point> best_poly(1);
            cv::approxPolyDP(best_guess, best_poly, 2, true);

            std::vector<std::vector<cv::Point > > drawable_contours(1);
            drawable_contours.push_back(best_poly);

            cv::drawContours(movement_buffer,
                    drawable_contours, -1,
                    cv::Scalar(255, 255, 255), CV_FILLED);
        }
    }
    else {
        movement_buffer.setTo(cv::Scalar::all(0));
    }
}

void rgb_captured(freenect_device *dev, void *rgb, uint32_t timestamp) {
    pthread_mutex_lock(&render_buffer_mutex);

    memcpy(bgr_buffer.data, rgb, WIDTH*HEIGHT*3);

    image_current.copyTo(image_prev);
    cv::cvtColor(bgr_buffer, image_current, CV_BGR2GRAY);

    detect_movement();

    if (started) {
        step_tracking();

        estimate_distance_to_features();

        firing_solution();
    }

    if (!started) { started = true;}

    pthread_mutex_unlock(&render_buffer_mutex);
}

void *capture_loop(void *arg) {
    freenect_set_depth_callback(device, depth_captured);
    freenect_set_depth_mode(device, freenect_find_depth_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_DEPTH_MM));
    freenect_start_depth(device);

    freenect_set_video_callback(device, rgb_captured);
    freenect_set_video_mode(device, freenect_find_video_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_VIDEO_RGB));
    freenect_set_video_buffer(device, kinect_rgb_buffer);
    freenect_start_video(device);

    while (freenect_process_events(context) >= 0) {
        if (die) break;
    }

    printf("shutting down streams...\n");
    freenect_stop_depth(device);
    freenect_close_device(device);
    freenect_shutdown(context);
    printf("done!\n");

    return NULL;
}

freenect_context* init_kinect() {
    freenect_context *context;

    if (freenect_init(&context, NULL) < 0) {
        printf("failed to initialize kinect\n");

        return NULL;
    } else {
        printf("initialized kinect\n");

        if (getenv("DEBUG")) {
            freenect_set_log_level(context, FREENECT_LOG_DEBUG);
        }
        freenect_select_subdevices(context, (freenect_device_flags)FREENECT_DEVICE_CAMERA);

        return context;
    }
}

void copy_image_to_rgb(cv::Mat &image, cv::Mat &display) {
    switch (image.type()) {
        case CV_8UC3:
            cv::cvtColor(image, display, CV_BGR2RGB);
            break;
        case CV_8UC1:
            cv::cvtColor(image, display, CV_GRAY2RGB);
            break;
    }
}

void graphics_loop(int argc, char **argv) {
    namedWindow("Topgun", cv::WINDOW_AUTOSIZE);

    cv::setMouseCallback("Topgun", mouse_events, NULL);

    cv::Mat *current_display = &bgr_buffer;
    cv::Mat display = cv::Mat(HEIGHT, WIDTH, CV_8UC3);

    int key;

    while (1) {
        pthread_mutex_lock(&render_buffer_mutex);

        copy_image_to_rgb(*current_display, display);

        draw_features_center(display);
        draw_current_points(display);
        draw_cursor_details(display);
        draw_distance_to_features(display);
        imshow("Topgun", display);

        pthread_mutex_unlock(&render_buffer_mutex);

        if ((key = cv::waitKey(30)) >= 0) {
            switch (key) {
                case 27: die = true; return; break;
                case 'd': current_display = &depth_buffer; break;
                case 'r': current_display = &bgr_buffer; break;
                case 'f': current_display = &foreground_mask; break;
                case 'm': current_display = &movement_buffer; break;
            }
        }
    }
}

void kill_kinect(int s){
    die = true;
    pthread_join(kinect_thread, NULL);

    exit(0);
}

void handle_sigint() {
    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = kill_kinect;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);
}

void init_cv_buffers() {
    bgr_buffer = cv::Mat(HEIGHT, WIDTH, CV_8UC3);

    depth_buffer = cv::Mat(HEIGHT, WIDTH, CV_8UC1);
    foreground_mask = cv::Mat(HEIGHT, WIDTH, CV_8UC1);
    movement_buffer = cv::Mat(HEIGHT, WIDTH, CV_8UC1);
    image_prev = cv::Mat(HEIGHT, WIDTH, CV_8UC1);
    image_current = cv::Mat(HEIGHT, WIDTH, CV_8UC1);

    background_subtractor = cv::BackgroundSubtractorMOG2(5, 16, true);
}

int main(int argc, char **argv) {
    init_cv_buffers();

    context = init_kinect();
    if (context == NULL) {
        return 1;
    }

    if (freenect_open_device(context, &device, 0) < 0) {
        printf("could not open device\n");
        freenect_shutdown(context);

        return 1;
    }

    int capture_loop_failed = pthread_create(&kinect_thread, NULL, capture_loop, NULL);
    if (capture_loop_failed) {
        printf("pthread_create failed\n");
        freenect_shutdown(context);

        return 1;
    }

    handle_sigint();

    graphics_loop(argc, argv);
    pthread_join(kinect_thread, NULL);
    printf("thanks for shooting down quads!\n");
}

