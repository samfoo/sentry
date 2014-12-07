.sensor:
	$(MAKE) -C sensor cv

.sentry:
	$(MAKE) -C sentry

clean:
	$(MAKE) -C sensor clean
	$(MAKE) -C sentry clean
