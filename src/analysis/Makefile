#

.PHONY: all clean

all: python

python: detect_iosig

clean:
	rm detect_iosig
	rm protoc_middleman signatureList_pb2.py
	rm *.pyc

protoc_middleman: signatureList.proto
	protoc --python_out=. signatureList.proto
	@touch protoc_middleman

detect_iosig: sig.py protoc_middleman
	@echo "Writing shortcut script detect_iosig..."
	@echo '#! /bin/sh' > detect_iosig
	@echo 'python ./sig.py "$$@"' >> detect_iosig
	@chmod +x detect_iosig

test:
	cp *.py /export/home/yyin/dev-test/iosig/trace_analyzer
	cp Makefile /export/home/yyin/dev-test/iosig/trace_analyzer
	cp *.properties /export/home/yyin/dev-test/iosig/trace_analyzer
	cp *.proto /export/home/yyin/dev-test/iosig/trace_analyzer
	cp README /export/home/yyin/dev-test/iosig/trace_analyzer
	cp -r trace_samples /export/home/yyin/dev-test/iosig/trace_analyzer

