#include "api-endpoints/emily.h"

using namespace std;
//bazel-bin/api-endpoints/emily-test-main

// int main() {
// // read file
// ifstream file("data.json");
// // json reader
// Json::Reader reader;
// // this will contain complete JSON data
// Json::Value completeJsonData;
// // reader reads the data and stores it in completeJsonData
// reader.parse(file, completeJsonData);
// // complete JSON data
// cout << "Complete JSON data: " << endl;
// // get the value associated with name key
// cout << "Name: "  << completeJsonData["name"] << endl;
// // get the value associated with grade key
// cout << "Grade: " << completeJsonData["grade"] << endl;
// return 0;
// }

#include <iostream>

//clear;bazel build //api-endpoints:emily-test-main
//bazel-bin/api-endpoints/emily-test-main


/**
 * \brief Parse a raw string into Value object using the CharReaderBuilder 
 * class, or the legacy Reader class.
 * Example Usage:
 * $g++ readFromString.cpp -ljsoncpp -std=c++11 -o readFromString
 * $./readFromString
 * colin
 * 20
 */
// int main() {
//   const std::string rawJson = R"({"Age": 20, "Name": "colin"})";
//   const auto rawJsonLength = static_cast<int>(rawJson.length());
//   constexpr bool shouldUseOldWay = false;
//   JSONCPP_STRING err;
//   Json::Value root;
 
//   if (shouldUseOldWay) {
//     Json::Reader reader;
//     reader.parse(rawJson, root);
//   } else {
//     Json::CharReaderBuilder builder;
//     const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
//     if (!reader->parse(rawJson.c_str(), rawJson.c_str() + rawJsonLength, &root,
//                        &err)) {
//       std::cout << "error" << std::endl;
//       return EXIT_FAILURE;
//     }
//   }
//   const std::string name = root["Name"].asString();
//   const int age = root["Age"].asInt();
 
//   std::cout << name << std::endl;
//   std::cout << age << std::endl;
//   return EXIT_SUCCESS;
// }

/** \brief Write the Value object to a stream.
 * Example Usage:
 * $g++ streamWrite.cpp -ljsoncpp -std=c++11 -o streamWrite
 * $./streamWrite
 * {
 *     "Age" : 20,
 *     "Name" : "robin"
 * }
 */
int main() {
  Json::Value root;
  Json::StreamWriterBuilder builder;
  const std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
 //jsonRootValue["emptyArray"] = Json::Value(Json::arrayValue);
 //jsonValue_ref.append(stringList[i]);
  Json::Value oneArray = Json::arrayValue;
  Json::Value anotherArray = Json::arrayValue;
  string strList[4] = {"a", "list", "of", "strings"};
  float fltList[4] = {5.0, 0.05, 0.25, 6.43};
  for (size_t i = 0; i < 4; i++) {
    oneArray.append(strList[i]);
    anotherArray.append(fltList[i]);

  }

 // anotherArray.append()
  root["Name"] = "robin";
  root["Age"] = 20;
  root["emptyArray"] = Json::Value(Json::arrayValue);
  root["oneArray"] = oneArray;
  root["anotherArray"] = anotherArray;
  
  writer->write(root, &std::cout);

  cout << "anotherArray is size " << root["anotherArray"].size() << '\n';
  cout << "|" << root["hello"].asString().length() << "|"  << endl;
  

  /*
  auto entriesArray = root["entries"];

    auto firstelem = entriesArray[0];
    float asFloat() const
  
  */
 
  return EXIT_SUCCESS;
}