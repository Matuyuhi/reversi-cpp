run:
ifeq ($(OS),Windows_NT)
	cmake.bat
else
	rm -rf build
	mkdir -p build
	cd build && cmake .. && make
	./build/work
endif


docker:
	docker compose up --build
clean:
	rm -rf build