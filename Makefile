.PHONY: test

test:
	python3 setup.py build_ext --inplace
	py.test tests
