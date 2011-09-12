#

.PHONY: all clean

all: python

python: detect_iosig

clean:
	rm -f detect_iosig
	rm -f protoc_middleman signatureList.pb.cc signatureList.pb.h signatureList_pb2.py edu/iit/iosig/IOSignatureList.java
	rm -f *.pyc
	rmdir edu/iit/iosig 2>/dev/null || true
	rmdir edu/iit 2>/dev/null || true
	rmdir edu 2>/dev/null || true

protoc_middleman: signatureList.proto
	protoc --python_out=. signatureList.proto
	@touch protoc_middleman

detect_iosig: sig.py protoc_middleman
	@echo "Writing shortcut script detect_iosig..."
	@echo '#! /bin/sh' > detect_iosig
	@echo 'python ./sig.py "$$@"' >> detect_iosig
	@chmod +x detect_iosig

