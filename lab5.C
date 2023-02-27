#include <iostream>
#include <fstream>
#include <cstdint>
#include <string>
#include <string.h>
#include <assert.h>
#include "String.h"

//perform unit tests on the String class
int main(int argc, char * argv[])
{

   // test the constructor and the getters
   String * str1 = new String((char *)"hello there");
   assert(strcmp((char *)"hello there", str1->get_cstr()) == 0);
   assert(strlen((char *)"hello there") == str1->get_length());
  
   std::string stdstr2("hello world");
   String * str2 = new String(stdstr2);
   assert(stdstr2.compare(str2->get_stdstr()) == 0);
   assert(stdstr2.length() == (uint32_t) str2->get_length());

   //test the isSpaces method
   String str3((char *)"a    phrase with    spaces ");
   bool error;
   assert(str3.isSpaces(1, 4, error) == true);
   assert(error == false);
   assert(str3.isSpaces(0, 4, error) == false);
   assert(error == false);
   assert(str3.isSpaces(1, 5, error) == false);
   assert(error == false);
   assert(str3.isSpaces(11, 11, error) == true);
   assert(error == false);
   assert(str3.isSpaces(4, 11, error) == false);
   assert(error == false);
   assert(str3.isSpaces(11, 12, error) == false);
   assert(error == false);
   assert(str3.isSpaces(17, 13, error) == false);
   assert(error == true);
   assert(str3.isSpaces(26, 26, error) == true);
   assert(error == false);
   assert(str3.isSpaces(26, 27, error) == false);
   assert(error == true);
   assert(str3.isSpaces(-2, -1, error) == false);
   assert(error == true);

   //test the convert2Hex method
   String str4((char *)"0x2aA: 123e5678");
   assert(str4.convert2Hex(2, 4, error) == 0x2aA);
   assert(error == false);
   assert(str4.convert2Hex(7, 8, error) == 0x12);
   assert(error == false);
   assert(str4.convert2Hex(9, 10, error) == 0x3e);
   assert(error == false);
   assert(str4.convert2Hex(11, 12, error) == 0x56);
   assert(error == false);
   assert(str4.convert2Hex(13, 14, error) == 0x78);
   assert(error == false);
   assert(str4.convert2Hex(14, 15, error) == 0);
   assert(error == true);
   assert(str4.convert2Hex(16, 17, error) == 0);
   assert(error == true);
   assert(str4.convert2Hex(8, 7, error) == 0);
   assert(error == true);
   assert(str4.convert2Hex(-1, 7, error) == 0);
   assert(error == true);
   assert(str4.convert2Hex(-2, -1, error) == 0);
   assert(error == true);

   String str5((char *)"0xabc: def01234");
   assert(str5.convert2Hex(2, 4, error) == 0xabc);
   assert(error == false);
   assert(str5.convert2Hex(7, 8, error) == 0xde);
   assert(error == false);
   assert(str5.convert2Hex(9, 10, error) == 0xf0);
   assert(error == false);
   assert(str5.convert2Hex(11, 12, error) == 0x12);
   assert(error == false);
   assert(str5.convert2Hex(13, 14, error) == 0x34);
   assert(error == false);

   String str6((char *)"0xABC: DEF56789");
   assert(str6.convert2Hex(2, 4, error) == 0xabc);
   assert(error == false);
   assert(str6.convert2Hex(7, 8, error) == 0xde);
   assert(error == false);
   assert(str6.convert2Hex(9, 10, error) == 0xf5);
   assert(error == false);
   assert(str6.convert2Hex(11, 12, error) == 0x67);
   assert(error == false);
   assert(str6.convert2Hex(13, 14, error) == 0x89);
   assert(error == false);

   String str7((char *)"0xAgC: DEi56789");
   assert(str7.convert2Hex(2, 4, error) == 0);
   assert(error == true);
   assert(str7.convert2Hex(9, 10, error) == 0);
   assert(error == true);

   //test the isChar method
   String str8((char *)"0xabc: abc123");
   assert(str8.isChar('0', 0, error) == true);
   assert(error == false);
   assert(str8.isChar('x', 1, error) == true);
   assert(error == false);
   assert(str8.isChar(':', 5, error) == true);
   assert(error == false);
   assert(str8.isChar(';', 5, error) == false);
   assert(error == false);
   assert(str8.isChar(';', -1, error) == false);
   assert(error == true);
   assert(str8.isChar(';', 13, error) == false);
   assert(error == true);

   //test the isSubString method passing it a char *
   assert(str8.isSubString((char *)"0x", 0, error) == true);
   assert(error == false);
   assert(str8.isSubString((char *)"abc", 2, error) == true);
   assert(error == false);
   assert(str8.isSubString((char *)"abc123", 7, error) == true);
   assert(error == false);
   assert(str8.isSubString((char *)"abc123", 5, error) == false);
   assert(error == false);
   assert(str8.isSubString((char *)"abc123", 8, error) == false);
   assert(error == true);
   assert(str8.isSubString((char *)"abc123", -1, error) == false);
   assert(error == true);

   //test the isSubString method passing it a std::string
   assert(str8.isSubString(std::string("0x"), 0, error) == true);
   assert(error == false);
   assert(str8.isSubString(std::string("abc"), 2, error) == true);
   assert(error == false);
   assert(str8.isSubString(std::string("abc123"), 7, error) == true);
   assert(error == false);
   assert(str8.isSubString(std::string("abc123"), 5, error) == false);
   assert(error == false);
   assert(str8.isSubString(std::string("abc123"), 8, error) == false);
   assert(error == true);
   assert(str8.isSubString(std::string("abc123"), -1, error) == false);
   assert(error == true);

   std::cout << "Congratuations!  All tests pass!\n"; 
}
