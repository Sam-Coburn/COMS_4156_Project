# cpplint style check
echo "running cpplint..."
cpplint \
--linelength=120 \
api-endpoints/* \
authentication/* \
main/* \
testing/* &> ./reports/style-check.txt

# flawfinder static analysis
echo "running flawfinder..."
flawfinder \
api-endpoints/* \
authentication/auth-service.h \
authentication/auth-service.cpp main/* \
testing/* &> ./reports/flawfinder.txt

# cppcheck static analysis
echo "running cppcheck..."
cppcheck --language=c++ api-endpoints/*.cc api-endpoints/*.cpp api-endpoints/*.h &> ./reports/cppcheck.txt      
cppcheck --language=c++ authentication/auth-service.h authentication/auth-service.cpp &>> ./reports/cppcheck.txt  
cppcheck --language=c++ main/*.cc main/*.cpp main/*.h &>> ./reports/cppcheck.txt 
cppcheck --language=c++ testing/*.cc testing/*.cpp testing/*.h &>> ./reports/cppcheck.txt 

# rerun regression tests
echo "running regression tests..."
bazel test //testing:test --test_output=all --test_env=GLOG_minloglevel=1 &> ./reports/test-report.txt

# bazel coverage report
echo "gathering coverage info..."
bazel coverage --instrumentation_filter=//api-endpoints:api-endpoints --combined_report=lcov //testing:test &> /dev/null
echo "generating coverage report..."
genhtml bazel-out/_coverage/_coverage_report.dat -o genhtml &> ./reports/coverage.txt