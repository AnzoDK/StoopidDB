/*
 StoopidDB format Created and owned by AnzoDK (Anton F. Rosenørn-Dohn) 2021
 
 IMPORTANT INFO FROM AnzoDK - There are two types of functions in this file: GET and SCAN.
 SCAN are meant to be check the whole buffer for the information required and GET are meant to ask the entry table (if applicable).
 If modifications to this file is made and/or new functions are created, it would be wise to keep this concept in mind to not confuse older users.
 
 */
#if defined (WIN32) || defined(WIN64)
typedef unsigned int uint;
typedef unsigned char u_int8_t; 
#endif
#pragma once
#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <streambuf>
#include <vector>
#include <unistd.h>
#include <algorithm>

#ifndef USE_KEYMAN //Check if we should use keyman.h or use the internal struct - Use the internal struct if in doubt
struct Key
{
    std::string name;
    std::string value;
    Key(const std::string &name, const std::string &value)
    {
        this->name = name;
        this->value = value;
    }
    /*
     * ONLY MEANT FOR ARRAY CREATION VIA THE NEW KEYWORD
     */
    Key()
    {
        name = "";
        value = "";
    }
    bool operator==(const Key &k)
    {
        if(name == k.name && value == k.value)
        {
          return true;  
        }
        return false;
    }
    bool operator!=(const Key &k)
    {
        if(name == k.name && value == k.value)
        {
          return false;  
        }
        return true;
    }
    Key(const Key& key)
    {
        name = key.name;
        value = key.value;
    }
};
#endif


#define TERMINAL_RED "\033[0;31m"
#define TERMINAL_NOCOLOR "\033[0m"
#define TERMINAL_CYAN "\033[0;36m"
#define TERMINAL_YELLOW "\033[0;33m"
#define TERMINAL_GREEN "\033[0;32m"

typedef unsigned char uchar;
uchar g_stoopidDBTableSig[8]{
           '\0',
           static_cast<uchar>('\xFF'),
           static_cast<uchar>('\xFF'),
           '\0',
           '\0',
           static_cast<uchar>('\xFF'),
           static_cast<uchar>('\xFF'),
           '\0'
           };
        
uchar g_stoopidDBEntrySig[6] = {
           'E',
           static_cast<uchar>('\xFF'),
           'N',
           static_cast<uchar>('\xFF'),
           'T',
           static_cast<uchar>('\xFF')
           };
uchar g_stoopidDBRowSig[6] = {
           'R',
           static_cast<uchar>('\0'),
           'O',
           static_cast<uchar>('\0'),
           'W',
           static_cast<uchar>('\0')
           };

std::vector<std::string> CppSplit(std::string str, char seperator) //I hate vectors for the simple reason that I like C-arrays - But I guess it makes sense here
//
{
   if(str.at(0) == seperator)
   {
       str.erase(0,1);
   }
   uint64_t last = 0;
   std::vector<std::string> arr = std::vector<std::string>();
   for(uint64_t i = 0; i < str.length();i++)
   {
     if(str.at(i) == seperator)
     {
         arr.push_back(str.substr(last,i-last));
         last = i+1;
     }
     if(i == str.length()-1)
     {
         arr.push_back(str.substr(last,i-last+1));
     }
   }
   return arr;
}

char** CSplit(const char* str, uint64_t strLen, char seperator, uint64_t &resultSize)
{
    uint64_t count = 0;
    for(uint64_t i = 0; i < strLen;i++)
    {
        if(str[i] == seperator)
        {
         count++;
        }
    }
    char** arr = new char*[count];
    uint64_t old = 0;
    for(uint64_t i = 0; i < strLen;i++)
    {
        if(str[i] == seperator)
        {
         arr[i] = new char[i-old];
         for(uint64_t u = 0; u < i-old;u++)
         {
             arr[i][u] = str[u];
         }
         old = i;
        }
    }
    resultSize = count;
    return arr;
}

void StrToLower(std::string &str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}


enum ColumnSettings
{
    NONE = 0b00000000,
    NOT_NULL = 0b10000000,
    AUTO_INCREMENT = 0b01000000,
    PRIMARY_KEY = 0b00100000
};

enum DataType
{
    INT = 0,
    VARCHAR = 1,
    TIME_DATE = 2
};


struct DBColumn
{
  std::string name = "";
  uint32_t size = 0;
  uchar settingsByte = 0x0;
  DataType type = INT;
  DBColumn()
  {
      
  }
  DBColumn(const std::string &_name, DataType _type, int _size, uchar _settingsByte)
  {
      name = _name;
      settingsByte = _settingsByte;
      size = _size;
      type = _type;
      
  }
};


class UnsignedBuffer
{
public:
    UnsignedBuffer()
    {
         m_ucBuffer = nullptr;
         length = 0;
    }
    explicit UnsignedBuffer(uint64_t size)
    {
        m_ucBuffer = new unsigned char[size];
        length = size;
    }
    UnsignedBuffer(char* s_buffer, int _length)
    {
        m_ucBuffer = new unsigned char[_length];
        
        length = _length;
        for(uint i = 0; i < length;i++)
        {
            m_ucBuffer[i] = static_cast<unsigned char>(s_buffer[i]);
        }
    }
    UnsignedBuffer(const char* s_buffer, uint64_t _length)
    {
        m_ucBuffer = new unsigned char[_length];
        
        length = _length;
        for(uint64_t i = 0; i < length;i++)
        {
            m_ucBuffer[i] = static_cast<unsigned char>(s_buffer[i]);
        }
    }
    UnsignedBuffer(const UnsignedBuffer &usBuff)
    {
      m_ucBuffer = new uchar[usBuff.length];
      length = usBuff.length;
      for(uint i = 0; i < length;i++)
      {
          m_ucBuffer[i] = usBuff.m_ucBuffer[i];
      }
    }
    UnsignedBuffer(uchar* buffer, uint64_t _length)
    {
        length = _length;
        m_ucBuffer = new uchar[length];
        for(uint64_t i = 0; i < length;i++)
        {
            m_ucBuffer[i] = static_cast<unsigned char>(buffer[i]);
        }
    }
    UnsignedBuffer& operator=(const UnsignedBuffer &ubuff)
    {
        if(this == &ubuff)
        {
          return *this;  
        }
        if(m_ucBuffer)
        {
          delete[] m_ucBuffer;  
        }
        length = ubuff.length;
        m_ucBuffer = new uchar[ubuff.length];
        for(uint i = 0; i < ubuff.length; i++)
        {
            m_ucBuffer[i] = ubuff.m_ucBuffer[i];
        }
        return *this;
    }
    unsigned char* GetBuffer()
    {
        return m_ucBuffer;
    }
    uchar* at(uint index)
    {
        
        return &m_ucBuffer[index];
    }
    char* ToSigned()
    {
        char* tmp_buffer = new char[length];
        for(int i = 0; i < length; i++)
        {
            tmp_buffer[i] = static_cast<char>(m_ucBuffer[i]);
        }
        return tmp_buffer;
    }
    void InsertAt(UnsignedBuffer buffer, uint64_t where)
    {
        for(uint64_t i = 0; i < buffer.length;i++)
        {
            m_ucBuffer[where+i] = buffer.GetBuffer()[i];
            if(where+i >= length-1)
            {
              break;  
            }
        }
    }
    static char* ToSigned(uint8_t* buffer, int _length)
    {
        char* tmp_buffer = new char[_length];
        for(int i = 0; i < _length; i++)
        {
            tmp_buffer[i] = static_cast<char>(buffer[i]);
        }
        return tmp_buffer;
    }
    static uchar* Resize(uchar* bufferToResize, uint oldSize, uint newSize)
    {
        uchar* tmpBuffer = new uchar[newSize];
        if(newSize < oldSize)
        {
            for(uint i = 0; i < newSize; i++)
            {
                tmpBuffer[i] = bufferToResize[i];
            }
        }
        else
        {
            for(uint i = 0; i < oldSize; i++)
            {
                tmpBuffer[i] = bufferToResize[i];
            } 
        }
        
        
       
        delete[] bufferToResize;
        return tmpBuffer;
    }
    bool operator==(const UnsignedBuffer &ubuff)
    {
        uint64_t c = 0;
        bool equal = 1;
        if(length != ubuff.length)
        {
            equal = 0;
            return equal;
        }
        while(true)
        {
            if(m_ucBuffer[c] != ubuff.m_ucBuffer[c])
            {
                equal = 0;
                break;
            }
            if(c == ubuff.length-1)
            {
               break; 
            }
            c++;
        }
        return equal;
    }
    void Resize(uint newSize)//Use m_PullData or m_PushData instead
    {
        
        
        uchar* tmpBuffer = new uchar[newSize];
        for(uint i = 0; i < length; i++)
        {
            tmpBuffer[i] = m_ucBuffer[i];
        }
        length = newSize;
        delete[] m_ucBuffer;
        m_ucBuffer = tmpBuffer;
       
    }
    static uchar* UcharFromString(std::string str)
    {
        uchar* buff = new uchar[str.length()];
        for(uint64_t i = 0; i < str.length();i++)
        {
            buff[i] = static_cast<uchar>(str.at(i));
        }
        return buff;
    }
    ~UnsignedBuffer()
    {
        if(m_ucBuffer)
        {
            delete[] m_ucBuffer;
            
        }
    }

    uint64_t length;
protected:
    unsigned char* m_ucBuffer;
};

class UnsignedString
{
public:
    
    int length()
    {
        uint64_t len = strBuffer.length;
        return len;
    }
    std::string ToSigned()
    {
        std::string tmp = "";
        for(uint64_t i = 0; i < strBuffer.length;i++)
        {
            tmp += *strBuffer.at(i);
        }
        return tmp;
    }
    explicit UnsignedString(const std::string &str)
    {
        strBuffer = UnsignedBuffer(str.c_str(),str.length());
    }
    UnsignedString(uchar c)
    {
        strBuffer = UnsignedBuffer(1);
        strBuffer.GetBuffer()[0] = c;
    }
    UnsignedString(const uchar buff[], uint64_t size)
    {
        strBuffer = UnsignedBuffer(size);
        for(uint64_t i = 0; i < size; i++)
        {
            *strBuffer.at(i) = buff[i];
        }
    }
    UnsignedString(char c)
    {
        strBuffer = UnsignedBuffer(1);
        strBuffer.GetBuffer()[0] = static_cast<uchar>(c);
    }
    UnsignedString(char* charBuff, int start, int end)
    {
        strBuffer = UnsignedBuffer(end-start);
        for(uint i = 0; i < end-start;i++)
        {
            *strBuffer.at(i) = static_cast<unsigned char>(charBuff[start+i]);
        }
    }
    UnsignedString(const char* c_str, int start, int end)
    {
        strBuffer = UnsignedBuffer(end-start);
        for(uint i = 0; i < end-start;i++)
        {
            *strBuffer.at(i) = static_cast<unsigned char>(c_str[start+i]);
        }
    }
    UnsignedString(const UnsignedString &uStr)
    {
        strBuffer = UnsignedBuffer(uStr.strBuffer);
    }
    UnsignedString(unsigned char* ucharBuff, int start, int end)
    {
        if(start >= end)
        {
          std::cout << "Invalid buffer" << std::endl;
          return;
        }
        strBuffer = UnsignedBuffer(ucharBuff, end-start);
    }
    explicit UnsignedString(UnsignedBuffer ucharBuff)
    {
        strBuffer = UnsignedBuffer(ucharBuff);
    }
    unsigned char at(uint i)
    {
        return strBuffer.GetBuffer()[i];
    }
    UnsignedBuffer GetBuffer()
    {
      return strBuffer;
    }
    UnsignedString& operator=(const UnsignedString &uStr)
    {
        if(this == &uStr)
        {
          return *this;  
        }
        strBuffer = UnsignedBuffer(uStr.strBuffer);
        return *this;
    }
    UnsignedString& operator+=(const UnsignedString &uStr)
    {
        UnsignedBuffer tmpBuffer = UnsignedBuffer(uStr.strBuffer.length+strBuffer.length);
        tmpBuffer.InsertAt(strBuffer,0);
        tmpBuffer.InsertAt(uStr.strBuffer,length());
        strBuffer = tmpBuffer;
        return *this;
    }
    bool operator==(std::string str)
    {
        int c = 0;
        bool equal = true;
        if(str.length() != length())
        {
            equal = 0;
            return equal;
        }
        while(true)
        {
          if(str.at(c) != static_cast<char>(strBuffer.GetBuffer()[c]))
          {
              equal = 0;
              break;
          }
          if(c >= length()-1)
          {
              break;
          }
         c++;
        }
        return equal;
    }
    bool operator==(UnsignedString ustr)
    {
        int c = 0;
        bool equal = true;
        if(ustr.length() != length())
        {
            equal = 0;
            return equal;
        }
        while(true)
        {
          if(ustr.at(c) != *strBuffer.at(c))
          {
              equal = 0;
              break;
          }
          if(c >= length()-1)
          {
              break;
          }
         c++;
        }
        return equal;
    }
    bool operator!=(UnsignedString ustr)
    {
        int c = 0;
        bool unequal = false;
        if(ustr.length() != length())
        {
            unequal = 1;
            return unequal;
        }
        while(true)
        {
          if(ustr.at(c) != strBuffer.GetBuffer()[c])
          {
              unequal = 1;
              break;
          }
          if(c >= length()-1)
          {
              break;
          }
         c++;
        }
        return unequal;
    }
    bool operator!=(std::string str)
    {
        int c = 0;
        bool unequal = false;
        if(str.length() != length())
        {
            unequal = 1;
            return unequal;
        }
        while(true)
        {
          if(str.at(c) != static_cast<char>(strBuffer.GetBuffer()[c]))
          {
              unequal = 1;
              break;
          }
          if(c >= length()-1)
          {
              break;
          }
         c++;
        }
        return unequal;
    }
    static uint64_t* Search(uchar* buffer, uint64_t offset, uint64_t length, uchar* searchTerm, uint64_t searchTermLength, int &resultSize)
    {
        UnsignedBuffer* buff1 = new UnsignedBuffer(searchTerm,searchTermLength);
        int c = 0;
        resultSize = 0;
        while(true)
        {
            
            UnsignedBuffer* buff2 = new UnsignedBuffer(&(buffer[c+offset]),searchTermLength);

            
            if(c+searchTermLength+offset >= length )
            {
                break;
            }
            else if(*buff1 == *buff2)
            {
               resultSize++;
            }
            c++;
            delete buff2;
            
        }
        uint64_t* indexes = new uint64_t[resultSize];
        c = 0;
        int cc = 0;
        while(true)
        {
            UnsignedBuffer* buff2 = new UnsignedBuffer(&(buffer[c+offset]),searchTermLength);
            if(c+searchTermLength+offset >= length )
            {
                break;
            }
            else if(*buff1 == *buff2)
            {
               
                indexes[cc] = c+offset;
                cc++;
            }
            c++;
            delete buff2;
        }
        delete buff1;
        return indexes;
    }
    static uint64_t* Search(uchar* buffer, uint64_t offset, uint64_t length, UnsignedString searchTerm, int &resultSize)
    {
        int c = 0;
        resultSize = 0;
        UnsignedBuffer* buff1 = new UnsignedBuffer(searchTerm.GetBuffer()); //The original buffer from SearchTerm gets fucked for some reason? (Code that caused the issue was: [ UnsignedBuffer* buff1 = new UnsignedBuffer(searchTerm.GetBuffer().GetBuffer(),searchTerm.length());]
        
        while(true)
        {
            
            UnsignedBuffer* buff2 = new UnsignedBuffer(&(buffer[c+offset]),searchTerm.length());
            if(c+searchTerm.length()+offset >= length )
            {
                break;
            }
            else if(*buff1 == *buff2)
            {
               resultSize++;
            }
            c++;
            delete buff2;
        }
        if(resultSize == 0)
        {
          return nullptr;  
        }
        uint64_t* indexes = new uint64_t[resultSize];
        
        c = 0;
        int cc = 0;
        while(true)
        {
            
            UnsignedBuffer* buff2 = new UnsignedBuffer(&(buffer[c+offset]),searchTerm.length());
            if(c+searchTerm.length()+offset >= length )
            {
                break;
            }
            else if(*buff1 == *buff2)
            {
               
                indexes[cc] = c+offset;
                cc++;
            }
            c++;
            delete buff2;
        }
        delete buff1;
        return indexes;
    }
    static uint64_t* SearchFromEnd(uchar* buffer, uint64_t offset, uchar* searchTerm, uint64_t searchTermLength, int &resultSize)
    {
        int c = 0;
        resultSize = 0;
        while(true)
        {
            if(static_cast<signed int>(offset-c-searchTermLength) < 0 )
            {
                break;
            }
            else if(UnsignedString(&buffer[offset-c-searchTermLength],0,searchTermLength) == UnsignedString(searchTerm,0,searchTermLength))
            {
               resultSize++;
            }
            c++;
        }
        uint64_t* indexes = new uint64_t[resultSize];
        c = 0;
        int cc = 0;
        while(true)
        {
            if(static_cast<signed int>(offset-c-searchTermLength) < 0 )
            {
                break;
            }
            else if(UnsignedString(&buffer[offset-c-searchTermLength],0,searchTermLength) == UnsignedString(searchTerm,0,searchTermLength))
            {
               
                indexes[cc] = offset-c-searchTermLength;
                cc++;
            }
            c++;
        }
        return indexes;
    }
    static uint64_t* SearchFromEnd(uchar* buffer, uint64_t offset, UnsignedString searchTerm, int &resultSize)
    {
        int c = 0;
        resultSize = 0;
        while(true)
        {
            if(static_cast<signed int>(offset-c-searchTerm.length()) < 0 )
            {
                break;
            }
            else if(UnsignedString(&buffer[offset-c-searchTerm.length()],0,searchTerm.length()) == searchTerm)
            {
               resultSize++;
            }
            c++;
        }
        uint64_t* indexes = new uint64_t[resultSize];
        c = 0;
        int cc = 0;
        while(true)
        {
            if(static_cast<signed int>(offset-c-searchTerm.length()) < 0 )
            {
                break;
            }
            else if(UnsignedString(&buffer[offset-c-searchTerm.length()],0,searchTerm.length()) == searchTerm)
            {
               
                indexes[cc] = offset-c-searchTerm.length();
                cc++;
            }
            c++;
        }
        return indexes;
    }
    ~UnsignedString()
    {

    }
private:
    UnsignedBuffer strBuffer;
};

struct DBRow
{
  std::string name = ""; //Only used for errors
  std::vector<Key> keys = std::vector<Key>(); 
  DBRow()
  {
      
  }
  DBRow(const DBRow &row)
  {
      for(size_t i = 0; i < row.keys.size();i++)
      {
          keys.push_back(row.keys.at(i));
      }
  }
  DBRow(std::string _name)
  {
      name = _name;
  }
  Key Find(std::string _name)
  {
     for(size_t i = 0; i < keys.size();i++)
     {
         if(keys.at(i).name == _name)
         {
           return keys.at(i);  
         }
     }
     return Key("","");
  }
  Key Find(Key k)
  {
      for(size_t i = 0; i < keys.size();i++)
      {
        if(k == keys.at(i))
        {
          return keys.at(i);  
        }
      }
      return Key("","");
  }
  void InsertData(std::string field, std::string data)
  {
      Key nKey = Key(field,data);
      keys.push_back(nKey);
  }
  void InsertData(Key k)
  {
      keys.push_back(k);
  }
  void InsertData(UnsignedString field, UnsignedString data)
  {
      std::string sF = "";
      std::string sD = "";
      sF.assign(field.GetBuffer().ToSigned(),field.length());
      sD.assign(data.GetBuffer().ToSigned(), field.length());
      Key nKey = Key(sF,sD);
      keys.push_back(nKey);
  }  
  
};

#ifndef NO_SQL
enum SQLResponseCode
{
  SQL_OK = 0b00000000,
  SQL_SYNTAX_ERROR = 0b10000000,
  SQL_NO_TABLE = 0b01000000,
  SQL_NO_COLUMN = 0b11000000,
  SQL_SCRIPT_ERROR = 0b00100000,
  SQL_NOT_IMPLEMENTED = 0b00010000
};

enum SQLCommand
{
  SELECT,
  UPDATE,
  DELETE,
  INSERT_INTO,
  CREATE,
  ALTER
};

enum SQLEntity
{
    TABLE,
    INDEX,
    DATABASE
};

struct SQLResponse
{
    SQLResponseCode code = SQL_OK;
    DBRow* returnedRows = 0x0;
    uint64_t returnedRowsSize = 0;
};
#endif

struct DataBase
{

   std::string path;
   std::string versionString;
   uint64_t DBSize;
   u_int8_t* DBBuffer = nullptr; 
   bool valid = true;
   std::string Err()
   {
       std::string errString = "";
       for(int i = 0; i < m_errorVector.size(); i++)
       {
           errString += m_errorVector.at(i);
           errString += "\n";
       }
       return errString;
   }
   DataBase(std::string dbPath)
   {
       path = dbPath;
       //read file
       std::ifstream in = std::ifstream(dbPath, std::ios::in | std::ios::binary);
       if(in)
       {
       in.seekg(0, std::ios::end);
       DBSize = in.tellg();
       in.seekg(0, std::ios::beg);
       char tmpBuff[DBSize];
       in.read(tmpBuff,DBSize);
       in.close();
       DBBuffer = new u_int8_t[DBSize];
       
       //Changing buffer to unsigned chars
       for(uint i = 0; i < DBSize; i++)
       {
           DBBuffer[i] = static_cast<unsigned char>(tmpBuff[i]);
       }
       //Read header
       std::string tmpString = "";
       tmpString.assign("StoopidDB\0\1\0",12);
       UnsignedString ustr = UnsignedString(tmpString);
       if(ustr != UnsignedString(DBBuffer, 0, tmpString.length()))
       {
           //Header is invalid
           valid = false;
           m_AddError("Invalid Header");
           return;
       }
       //std::cout << "Fetching Database Info..." << std::endl;
       //std::stringstream dbInfoStream = std::stringstream();
       //dbInfoStream << "DBEntryHeader @ 0x" << std::setfill('\0');
       //for(int i = 0; i < 8;i++)
       //{
           //dbInfoStream << std::setw(sizeof(uchar)*2);
           //dbInfoStream << std::hex << (int)DBBuffer[DBSize-8+i];
           
       //}
       //std::string dbInfoString = std::string(dbInfoStream.str());
       //std::cout << dbInfoString << std::dec << "\0" << std::endl;
       //std::cout << std::endl;
       
       }
       else
       {
         valid = false;
         m_errorVector = std::vector<std::string>();
         m_AddError("Could not open file");
         return;
       }
       
       
       
   }
   ~DataBase()
   {
       if(DBBuffer != nullptr)
       {
        delete[] DBBuffer;
       }
   }
   private:
    std::vector<std::string> m_errorVector;
    void m_AddError(std::string err)
    {
      m_errorVector.push_back(err);
    }
};

class DBManager
{
public:
   DBManager(const std::string& path)
   {
       m_dbPath = path;
       LoadDB();
   }
   DBManager()
   {
       m_dbPath = "";
       
   }
   std::string Error()
   {
      std::string RetString = "";
      std::string DBError = m_currDB->Err();
      std::string DBManError = "";
      
      if(m_errorVector.size() > 0)
      {
          RetString += "DBManager errors : ";
          RetString += m_errorVector.size(); 
          RetString += "\n";
      
          for(int i = 0; i < m_errorVector.size(); i++)
          {
            RetString += m_errorVector.at(i) + "\n";
          }
      }
      else
      {
        return "";  
      }
      
      if(DBError != "")
      {
          RetString += "DataBase Error: " + DBError;
      }
      
      
      
      return RetString; 
   }
   
   void CreateDB(const std::string &path)
   {
       std::ofstream out = std::ofstream(path, std::ofstream::binary);
       if(out)
       {
           std::string sigString = "";
           sigString.assign("StoopidDB\0\1\0\0\xFF\xFF\0\0\xFF\xFF\0\xC\0\0\0\0\0\0\0",28);
           out.write(sigString.c_str(),sigString.length());
           out.close();
       }
       else
       {
           std::cout << "File could not be created - Check permissions" << std::endl;
           return;
       }
   }
   bool LoadDB()
   {
        if(m_currDB != nullptr) //memleaks for the db but as the db has not loaded any files into memory - it is very minor
        {
            if(m_currDB->DBBuffer != nullptr)
            {
                delete (m_currDB);
            }
        }
        m_currDB = new DataBase(m_dbPath);
        if(m_dbPath != "")
        {
            if(!m_currDB->valid)
            {
                std::cout << "\033[0;31m" << "Invalid DB or Missing DB" << "\033[0m" << std::endl;
                m_dbPath = "";
                if(m_currDB->DBBuffer != nullptr) //memleaks for the db but as the db has not loaded any files into memory - it is very minor
                {
                    delete (m_currDB);
                    m_currDB = nullptr;
                }
                return false;
                
            }
            
        }
        else
        {
          std::cout << "No DBPath - Ignoreing Load request" << std::endl; //Should maybe be replaced with a m_AddError();
          return false;
        }
        return true;
        
   }
   bool LoadDB(const std::string &path)
   {
       m_dbPath = path;
       return LoadDB();
   }
   bool CreateTable(const std::string &tableName, int columnCount, DBColumn* columns)
   {
       if(m_currDB == nullptr)
       {
           std::cout << TERMINAL_RED << "Can't create table - No DB is loaded" << TERMINAL_NOCOLOR << std::endl;
           return 0;
       }
       if(BackUp() != 0)
       {
           std::cout << TERMINAL_RED << "Can't create table - Could not backup Database" << TERMINAL_NOCOLOR << std::endl;
           return 0;
       }
       uint64_t entryOffset = GetEntryPointer();
       uint64_t bodyOffset = 12;
       uint64_t bodyLength = entryOffset-bodyOffset;
       uint64_t allowedWriteOffset = bodyOffset+bodyLength; //Same offset as the table start, so we'll use it for the creation of the index table
       //uint64_t extraBuffSize = m_currDB->DBSize-allowedWriteOffset;
       uint64_t extraBuffSize = 0;
       uint64_t sigOffset = 0;
       uint64_t c = 0;
       
       while(true)
       {
           if(c+8 < m_currDB->DBSize-allowedWriteOffset)
           {
                if( UnsignedString(&m_currDB->DBBuffer[allowedWriteOffset+c],0,8) == UnsignedString(g_stoopidDBTableSig,0,8))
                {
                    extraBuffSize = c;
                    sigOffset = c+allowedWriteOffset;
                    break;
                }
                else
                {
                    c++;
                }
           }
           else
           {
             //No sig?
             break;  
           }
       }
       
       if(sigOffset == 0)
       {
           m_AddError(std::string(TERMINAL_RED) + "No signature found to indicate the end of entry table" + TERMINAL_NOCOLOR);
           return 1;
       }
       
       int expectedExtraLength = 0;
       expectedExtraLength += (tableName.length()+1 < 256 ? tableName.length()+1 : 255); //Do not exceed 255
       uint64_t EOTOffset = expectedExtraLength+allowedWriteOffset;
       expectedExtraLength += 8; //for EOT
       uint64_t columnCountOffset = expectedExtraLength+allowedWriteOffset;
       expectedExtraLength += 4; //for amount of columns
       uint64_t PrevColumnEndOffset = columnCountOffset+4;
       uint64_t tableLength = 0;
       tableLength += (tableName.length()+1 < 256 ? tableName.length()+1 : 255);
       tableLength += 8; //EOTLength
       tableLength += 4; //for amount of columns
       if(entryOffset == 0)
       {
           std::cout << TERMINAL_RED << "Failed to get entry table offset" << TERMINAL_NOCOLOR << std::endl;
           return 0;
       }
       uint64_t combinedSize = expectedExtraLength;
       for(int i = 0; i < columnCount; i++)
       {
           combinedSize += (columns[i].name.length()+1 < 256 ? columns[i].name.length()+1 : 255); //Do not exceed 255
           combinedSize += 4; //for columnSizeOffset
           combinedSize += 2; //for DataTypeByte and settingsByte
           combinedSize += 4; //for maxDataSize
       }
       std::cout << "Resizing DB from " << m_currDB->DBSize << " to " << combinedSize+m_currDB->DBSize << std::endl;
       ResizeDB(combinedSize+m_currDB->DBSize);
       
       //Relocate old data - Must be done backwards to avoid data overwriting itself when tables are smaller than the entry table XD
       for(uint64_t u = 0; u < m_currDB->DBSize-allowedWriteOffset-combinedSize;u++)
       {
          m_currDB->DBBuffer[m_currDB->DBSize-1-u] = m_currDB->DBBuffer[allowedWriteOffset+(m_currDB->DBSize-allowedWriteOffset-combinedSize-1)-u];
       }
       m_NullFill(allowedWriteOffset,combinedSize);
       for(int i = 0; i < columnCount; i++)
       {
           UnsignedString uNameString = UnsignedString(columns[i].name);
           
           uint32_t columnStart = expectedExtraLength;
           expectedExtraLength += (columns[i].name.length()+1 < 256 ? columns[i].name.length()+1 : 255); //Do not exceed 255
           uint64_t columnSizeOffset = expectedExtraLength+allowedWriteOffset;
           expectedExtraLength += 4; //for columnSizeOffset
           
           uint64_t DataTypeOffset = columnSizeOffset+4;
           uint64_t SettingsByteOffset = DataTypeOffset+1;
           uint64_t MaxDataOffset = SettingsByteOffset+1;
           
           
           expectedExtraLength += 2; //for DataTypeByte and settingsByte
           expectedExtraLength += 4; //for maxDataSize
           uint32_t columnSize = expectedExtraLength-columnStart;
           
           

           for(int u = 0; u < (columns[i].name.length()+1 < 256 ? columns[i].name.length()+1 : 255); u++)
           {
               
               if(u != 254 && u == (columns[i].name.length()+1 < 256 ? columns[i].name.length()+1 : 255)-1 )
               {
                    m_currDB->DBBuffer[PrevColumnEndOffset+u] = static_cast<uchar>('\0');
               }
               else
               {
                    m_currDB->DBBuffer[PrevColumnEndOffset+u] = uNameString.at(u);  
               }
           }
           
           m_currDB->DBBuffer[DataTypeOffset] = static_cast<uchar>(columns[i].type);
           m_currDB->DBBuffer[SettingsByteOffset] = columns[i].settingsByte;
           uchar* sizePtr = (uchar*)&columns[i].size;
           for(int u = 0; u < 4;u++)
           {
               m_currDB->DBBuffer[MaxDataOffset+u] = sizePtr[u];
               m_currDB->DBBuffer[columnSizeOffset+u] = (uchar)((uchar*)&columnSize)[u];
           }
           
           PrevColumnEndOffset = PrevColumnEndOffset+columnSize;
           tableLength += columnSize;
           
       }
       uint64_t ETO = allowedWriteOffset+tableLength;
       for(int i = 0; i < 8;i++)
       {
           m_currDB->DBBuffer[EOTOffset+i] = (uchar)((uchar*)&ETO)[i];
       }
       for(int i = 0; i < 4; i++)
       {
           m_currDB->DBBuffer[columnCountOffset+i] = (uchar)((uchar*)&columnCount)[i];
       }
       
       for(int i = 0; i < (tableName.length()+1 < 256 ? tableName.length()+1 : 255); i++)
       {
          
          if(i != 254 && i == (tableName.length()+1 < 256 ? tableName.length()+1 : 255)-1 )
           {
               m_currDB->DBBuffer[allowedWriteOffset+i] = static_cast<uchar>('\0');
           }
           else
           {
             m_currDB->DBBuffer[allowedWriteOffset+i] = tableName.at(i); 
           }
          
       }
       uint64_t newTableOffset = (allowedWriteOffset+tableLength);
       int size = 0;
       uint64_t newEntryOffset = 0;
       uint64_t* results = UnsignedString::Search(m_currDB->DBBuffer, allowedWriteOffset+tableLength, m_currDB->DBSize, g_stoopidDBEntrySig, 6, size);
       
       std::cout << "Found " << size << " other tables" << std::endl;
       m_WriteDBToDisk();
       uint64_t ENTWrite = 0;
       if(size > 0)
       {
           for(uint64_t i = 0; i < size;i++)
           {
               uint64_t entrySizeOffset = results[i]+6; //6 = length of ENT siganture
               uint32_t entrySize32_t = m_ReadUnsingedValue(4,entrySizeOffset);
               ENTWrite = results[i]+entrySize32_t;
               newEntryOffset += entrySize32_t;
           }
       }
       newEntryOffset+=newTableOffset;
       
       int extraSize = 0;
       extraSize += (tableName.length()+1 < 256 ? tableName.length()+1 : 255);
       int tableNameSize = (tableName.length()+1 < 256 ? tableName.length()+1 : 255);
       extraSize += 4+8+6+8; //4 for length of entry, 8 for offset, 6 for ENT signature and 8 for the EntryTableEndSig
       std::cout << "Resizing DB from " << m_currDB->DBSize << " to " << m_currDB->DBSize+extraSize << std::endl;
       ResizeDB(m_currDB->DBSize+extraSize);
       
       if(ENTWrite == 0)
       {
          ENTWrite = newTableOffset;
       }
       
       for(int i = 0; i < 6;i++)
       {
           m_currDB->DBBuffer[ENTWrite+i] = g_stoopidDBEntrySig[i];
       }
       uint32_t ENTSize = static_cast<uint32_t>(extraSize)-8; 
       for(int i = 0; i < 4; i++)
       {
            m_currDB->DBBuffer[ENTWrite+6+i] = (uchar)((uchar*)&ENTSize)[i];
       }
       
       for(int i = 0; i < tableNameSize;i++)
       {
           
           if(i != 254 && i == tableNameSize-1 )
           {
              m_currDB->DBBuffer[ENTWrite+6+4+i+1] = static_cast<uchar>('\0');
           }
           else
           {
               m_currDB->DBBuffer[ENTWrite+6+4+i] = static_cast<uchar>(tableName.at(i));
           }
       }
       
       for(int i = 0; i < 8; i++)
       {
            m_currDB->DBBuffer[ENTWrite+6+4+tableNameSize+i] = (uchar)((uchar*)&allowedWriteOffset)[i];
       }
       
       for(int i = 0; i < 8;i++)
       {
           m_currDB->DBBuffer[ENTWrite+6+4+tableNameSize+8+i] = g_stoopidDBTableSig[i];
       }
       
       
       if(m_currDB->DBSize+8-1 > ENTWrite+extraSize+8)
       {
           std::cout << "Excessive amounts of NULL's detected - Shrinking DB from " << m_currDB->DBSize << " to " << ENTWrite+extraSize+8 << std::endl;
           ResizeDB(ENTWrite+extraSize+8);
       }
       
       
       uchar* p = (uchar*)&newTableOffset;
       for(int i = 0; i < 8;i++)
       {         
            m_currDB->DBBuffer[m_currDB->DBSize-8+i] = p[i];
       }
       
       
       
       std::ofstream out = std::ofstream(m_dbPath,std::ios::binary);
       std::string s_out = "";
       s_out.assign(UnsignedBuffer::ToSigned(m_currDB->DBBuffer,m_currDB->DBSize),m_currDB->DBSize);
       out.write(s_out.c_str(),m_currDB->DBSize);
       out.close();
       delete[] results;
       return 1;
   }
   
   DBRow* GetAllRowsFromTable(std::string tableName, int &resultSize)
   {
       uint64_t exist = TableExist(tableName);
       if(!exist)
       {
        return notable;
       }
       else
       {
            DBRow* dbRows = m_GetAllRows(tableName);
            resultSize = m_GetRowCount(tableName);
            return dbRows;
       }
   }
   /*
    * Key* Conditions format: An array of conditions in the form of a key with a name and a value, where the name is the columnName and the value is the value of the column
    */
   DBRow* GetRowsWhere(std::string tableName, Key* conditions, size_t conditionsSize, uint64_t &resultSize)
   {    
       uint64_t exist = TableExist(tableName);
       if(!exist)
       {
           resultSize = 0;
           return notable;
       }
        std::vector<DBRow> rows = std::vector<DBRow>();
        DBRow* dbRows = m_GetAllRows(tableName);
        uint64_t _resultSize = m_GetRowCount(tableName);
        for(int i = 0; i < _resultSize; i++)
        {
            uint64_t match = 0;
            for(size_t u = 0; u < conditionsSize; u++)
            {
                Key tmpKey = dbRows[i].Find(conditions[u].name);
                if(tmpKey != *nokey)
                {
                    if(tmpKey.value == conditions[u].value)
                    {
                        match++;
                    }
                }
            }
            if(match == conditionsSize)
            {
                rows.push_back(dbRows[i]);
            }
        }
        return m_ArrayFromVector(rows,resultSize);
       
   }
   
   bool InsertRow(std::string tableName, DBRow row)
   {
       uint64_t offset = TableExist(tableName);
       if(offset != 0)
       {
           uint64_t EOT = m_GetEOT(tableName);
           uint64_t expandSize = 0;
           for(int i = 0; i < row.keys.size();i++)
           {
               uint32_t maxSize = m_GetColumnDataMaxSize(tableName,row.keys.at(i).name);
               if( maxSize == 0 )
               {
                   m_AddError("Column: '" + row.keys.at(i).name + "' Could not be found");
                   return 0;
               }
               expandSize += maxSize;
           }
           expandSize+= 6+8; //For the rowsig and row size
           expandSize+= row.keys.size()*4; //reserve space for columnID
           expandSize+=2; //this is needed - have yet to know why
           expandSize += m_CalculateSpaceForUnassignedColumns(tableName,row.keys);
           uint64_t newEOT = EOT+expandSize;  
           m_RewriteEOT(tableName,newEOT);
           ResizeDB(expandSize+m_currDB->DBSize,1);
           m_PushData(EOT,expandSize);
           m_NullFill(EOT,expandSize);
           m_UpdateEntryTableOffset(expandSize);
           m_FixEntryTable(expandSize);
           //uint64_t writeOffset = m_GetLastColumnEndOffset(tableName);
           uint64_t writeOffset = m_GetNewRowWriteOffset(tableName);
           m_DirectWrite(writeOffset, 6, g_stoopidDBRowSig);
           m_DirectWrite(writeOffset+6,8,(uchar*)&expandSize);
           uint64_t newWriteOffset = writeOffset+6+8;
           if(!m_WriteAutoAndUpdateOffset(tableName,newWriteOffset))
           {
               return false;
           }
           for(int i = 0; i < row.keys.size();i++)
           {
               uint32_t id = m_GetColumnID(tableName,row.keys.at(i).name);
               uint32_t maxData = m_GetColumnDataMaxSize(tableName,row.keys.at(i).name);
               if(id != 0xFFFFFFFF)
               {
                    m_DirectWrite(newWriteOffset,4,(uchar*)&id);
                    newWriteOffset+=4;
                    uint64_t value = 0;
                    DataType type = DataType(m_GetColumnType(tableName,row.keys.at(i).name));
                    switch(type)
                    {
                        case DataType::INT:
                        {
                          if(m_GetColumnDataMaxSize(tableName,row.keys.at(i).name) <= 4 )  
                          {
                            value = static_cast<uint32_t>(std::stoi(row.keys.at(i).value));
                          }
                          else if(m_GetColumnDataMaxSize(tableName,row.keys.at(i).name) <= 8 && m_GetColumnDataMaxSize(tableName,row.keys.at(i).name) > 4)
                          {
                              
                              for(uint64_t u = 0; u < row.keys.at(i).value.length();u++)
                              {
                                  char c = row.keys.at(i).value.at(row.keys.at(i).value.length()-1-u);
                                  std::string tmp = "";
                                  tmp += c;
                                  if(u == 0)
                                  {
                                    value += static_cast<uint64_t>(std::stoi(tmp));
                                  }
                                  else
                                  {
                                    value += static_cast<uint64_t>(std::stoi(tmp)*(10*u));  
                                  }
                              }
                          }
                           if(row.keys.at(i).value.length() < maxData)
                            {
                                m_DirectWriteNullFill(newWriteOffset,row.keys.at(i).value.length(),maxData,(uchar*)&value);
                            }
                            else
                            {
                                m_DirectWrite(newWriteOffset,maxData,(uchar*)&value);
                                m_AddError("Data is too large for column - Writing untill maxSize is hit");
                            }
                          break;  
                        }
                        case DataType::VARCHAR:
                        {
                            if(row.keys.at(i).value.length() < maxData)
                            {
                                m_DirectWriteNullFill(newWriteOffset,row.keys.at(i).value.length(),maxData,UnsignedBuffer::UcharFromString(row.keys.at(i).value));
                            }
                            else
                            {
                                m_DirectWrite(newWriteOffset,maxData,UnsignedBuffer::UcharFromString(row.keys.at(i).value));
                                m_AddError("Data is too large for column - Writing untill maxSize is hit");
                            }
                          break;  
                        }
                        case DataType::TIME_DATE: //TODO
                        {
                            break;
                        }
                    }

                    newWriteOffset+=maxData;
                    
               }
               else
               {
                   std::string err = std::string("Could not get id of column: ");
                   err += row.keys.at(i).name;
                   m_AddError(err); 
                   return 0;
               }
           }
           
           m_WriteDBToDisk();
           return 1;
           

           
       }
       else
       {
           m_AddError("Table: '" + tableName + "' Could not be found");
           return 0;  
       }
   }
   /*
    * @param Key where is should contain a column name and a value to look for and if they are equal - the data in Key data will be inserted into the requested row
    */
   bool ModifyRow(std::string tableName, Key where, Key data)
   {
       DBRow* rows = m_GetAllRows(tableName);
       uint64_t rowCount = m_GetRowCount(tableName);
       for(uint64_t i = 0; i < rowCount; i++)
       {
           Key tmpK = rows[i].Find(where);
           if(tmpK != *nokey)
           {
               uint64_t offset = m_GetRowOffsetByIndex(tableName,i);
               if(offset == 0)
               {
                 m_AddError("[Fatal] Could not find row that was supposed to exist");
                 return 0;
               }
               uint64_t length = m_GetRowLength(offset);
               uint32_t id = m_GetColumnID(tableName,data.name);
               offset+=8;
               while(true)
               {
                  uint32_t readID = m_ReadUnsingedValue(4,offset);
                  offset+=4;

                  if(id == readID)
                  {
                    if(data.value.length() > m_GetColumnDataMaxSize(tableName,data.name))
                    {
                      m_AddError("[Non-Fatal] data is longer than max data size");
                    }  
                    m_DirectWriteNullFill(offset,data.value.length(), m_GetColumnDataMaxSize(tableName,data.name), data.value.c_str());
                    return 1;
                  }
                  offset+=m_GetColumnDataMaxSize(tableName,m_GetColumnNameFromID(tableName,readID));
                  if(offset > length)
                  {
                     break; 
                  }
               }
           }
       }
       return false;
   }
   
   bool DeleteTable(std::string tableName) //I mean who needs tables anyway XD
   {
       //Get the offset and end of the table
       uint64_t startOffset = TableExist(tableName);
       
       if(startOffset != 0)
       {
            uint64_t endOffset = m_GetEOT(tableName);
            //Sanity test
            if(endOffset <= startOffset)
            {
                m_AddError("Offsets are completly lost - Try a table rebuild");
                return 0;
            }
            uint64_t size = endOffset-startOffset;
            //Repair all other table EOT's before pulling the data - This fixes wrong EOT reads when a table is deleted, and an EOT is read from a table that used to be further down in the data
            m_ModifyEOT(-1,-size); //-1 means all tables - this will also effect the table we're currently deleting, but as we are deleting it, it doesn't matter
            m_PullData(startOffset,size);
            m_NullFill(m_currDB->DBSize-size-1,size); //Not necessary, but makes it easier to see the changes if the DB file is inspected in a hex-editor
            m_WriteDBToDisk(); //DEBUG
            //Resize should clear space for both the old entry and table
            //ResizeDB(m_currDB->DBSize-size-(tableName.length()+1+8),1); //+1 for NULL terminator and + 8 for OFFSET
            ResizeDB(m_currDB->DBSize-size);
            m_FixEntryTable(-size);
       }
       else
       {
           m_AddError("Could not find table");
           return 0;
       }
       m_WriteDBToDisk();
       return 1;
       
       
   }
   
   bool DeleteRow(std::string tableName, Key where) //TODO Add support for multiple conditions //Lol ofc XDXDXDXD
   {
       DBRow* rows = m_GetAllRows(tableName);
       uint64_t rowCount = m_GetRowCount(tableName);
       for(uint64_t i = 0; i < rowCount; i++)
       {
           Key tmpK = rows[i].Find(where);
           if(tmpK != *nokey)
           {
               uint64_t offset = m_GetRowOffsetByIndex(tableName,i);
               if(offset == 0)
               {
                 m_AddError("[Fatal] Could not find row that was supposed to exist");
                 return 0;
               }
               uint64_t length = m_GetRowLength(offset);
               for(uint64_t i = 0; i < m_currDB->DBSize-offset;i++)
               {
                   m_currDB->DBBuffer[offset+i] = m_currDB->DBBuffer[offset+length+i];
               }
               ResizeDB(m_currDB->DBSize-length,1);
               return 1;
               
           }
       }
       return false;
   }
   #ifndef NO_SQL
   SQLResponse SQlQuery(std::string sqlQ,bool verbose=false)
   {
       SQLResponse response = SQLResponse();
       if(SqlReturn != nullptr)
       {
            delete[] SqlReturn;
            SqlReturnLength = 0;
            SqlReturn = nullptr;
       }
       SqlReturnLength = 0;
       if(verbose)
       {
           std::cout << TERMINAL_CYAN << '"' << "PlsNoSQL v1 C++ edition" << '"' << " will be used for SQL Processing." << std::endl << "The keywords and syntax may differ a bit from usual SQL." << std::endl << "Please refer to the documentation for help." << TERMINAL_NOCOLOR << std::endl;
       }
       size_t pos = sqlQ.find("'", 0);
       std::vector<size_t> indexes = std::vector<size_t>();
       while(pos != std::string::npos)
       {
            indexes.push_back(pos);
            pos = sqlQ.find("'",pos+1);
       }
       if(indexes.size() % 2 != 0)
       {
           std::string s = TERMINAL_YELLOW;
           s += "[Fatal] {SQL Code Processing} Un-even amount of (') symbols - Note that ";
           s += '"';
           s += " is not supported";
           s += TERMINAL_NOCOLOR;
           m_AddError(s);
           response.code = SQLResponseCode::SQL_SYNTAX_ERROR;
       }
       pos = sqlQ.find(";", 0);
       indexes.clear();
       while(pos != std::string::npos)
       {
            indexes.push_back(pos);
            pos = sqlQ.find(";",pos+1);
       }
       if(indexes.size() == 0)
       {
           response.code = SQL_SYNTAX_ERROR;
           m_AddError("[Fatal] {SQL Code Processing} Missing ';'");
           return response;
       }
       if(verbose)
       {
         std::cout << TERMINAL_CYAN << "[Info] {SQL Code Processing} " << indexes.size() << " Line(s) detected" << TERMINAL_NOCOLOR << std::endl;
         std::cout << TERMINAL_CYAN << "[Info] {SQL Code Processing} Starting Keyword Detection..." << TERMINAL_NOCOLOR << std::endl;
       }
       //Keyword detection
       std::vector<std::string> lines = std::vector<std::string>();
       size_t prevIndex = 0;
       for(size_t i = 0; i < indexes.size();i++)
       {
           lines.push_back(sqlQ.substr(prevIndex,indexes.at(i)+1));
           prevIndex = indexes.at(i)+1;
       }
       indexes.clear();
       std::vector<SQLCommand> CommandKeys = std::vector<SQLCommand>();
       std::vector<SQLEntity> AffectedEntities = std::vector<SQLEntity>();
       std::vector<std::string> Names = std::vector<std::string>();
       size_t FROM = 0;
       for(size_t i = 0; i < lines.size();i++)
       {
           std::vector<std::string> words = CppSplit(lines.at(i),' ');
           std::transform(words.at(0).begin(), words.at(0).end(), words.at(0).begin(), ::tolower);
           if(words.at(0) == "select")
           {
               CommandKeys.push_back(SQLCommand::SELECT);
               std::string* columnNames = nullptr;
               uint64_t columnNamesSize = 0;
               int columnNameEndWord = -1;
               for(size_t u = 0; u < words.size();u++)
               {
                   if(words.at(u).find(")") != std::string::npos)
                   {
                       columnNameEndWord = u;
                       break;
                   }
                   if(words.at(u).find("FROM") != std::string::npos) //For future me - check for lower case "FROM" as well
                   {
                     FROM = u;  
                     break;
                   }
               }
               if(columnNameEndWord == -1)
               {
                   columnNames = new std::string[1];
                   columnNames[0] = words.at(1);
                   columnNamesSize = 1;
               }
               else
               {
                   std::string tmpColumnNameString = "";
                   for(size_t u = 1; u < columnNameEndWord+1;u++)
                   {
                       tmpColumnNameString += words.at(u);
                   }
                   //Prevent empty parathesis
                   if(tmpColumnNameString == "" || tmpColumnNameString == "()")
                   {
                       response.code = SQL_SYNTAX_ERROR;
                       m_AddError("No column names specified");
                       return response;
                   }
                   if(verbose)
                   {
                       std::cout << TERMINAL_CYAN << "[Info] {SQL Code Processing} Combined words from input - Assuming string: "<< '"' << tmpColumnNameString << '"' << " Contains the columnNames" << TERMINAL_NOCOLOR << std::endl;
                   }
                   int tmpPos = 0;
                   /* //This has already been done by the CppSplit function - Keeping it in case it might be needed due to some weird bug or something
                   while(tmpColumnNameString.find(" ",tmpPos) != std::string::npos)
                   {
                       int tmpIndex = tmpColumnNameString.find(" ",tmpPos);
                       tmpColumnNameString.erase(tmpIndex,1);
                       tmpPos = tmpIndex+1;
                   }*/
                   tmpColumnNameString.erase(0,1); //Delete the '('
                   tmpColumnNameString.erase(tmpColumnNameString.length()-1,1); //Delete the ')'
                   tmpPos = 0;
                   std::vector<std::string> columnNamesVec = std::vector<std::string>();
                   while(tmpColumnNameString.find(",",tmpPos) != std::string::npos)
                   {
                       int tmpIndex = tmpColumnNameString.find(",",tmpPos);
                       columnNamesVec.push_back(tmpColumnNameString.substr(tmpPos,tmpIndex-tmpPos));
                       tmpPos = tmpIndex+1;
                   }
                   columnNamesVec.push_back(tmpColumnNameString.substr(tmpPos));
                   if(verbose)
                   {
                       std::cout << TERMINAL_CYAN << "[Info] {SQL Code Processing} Assuming columns to be: {" << std::endl;
                       for(size_t u = 0; u < columnNamesVec.size();u++)
                       {
                           std::cout << columnNamesVec.at(u) << std::endl;
                       }
                       std::cout << "}" << TERMINAL_NOCOLOR << std::endl;
                   }
                   columnNames = m_ArrayFromVector(columnNamesVec,columnNamesSize);
                   //TODO Return rows and check for "WHERE" - but first Imma create a function to get specific rows from the database - DONE (Now it's time to create the SQL handle for SELECT)
                   
                   
               }
               if(FROM == 0)
               {
                   for(size_t z = columnNameEndWord; z < words.size();z++)
                   {
                     if(words.at(z) == "FROM")//TODO check for lowercase as well
                     {
                         FROM = z;
                         break;
                     }
                   }
                   if(FROM == 0)
                     {
                         response.code = SQL_SYNTAX_ERROR;
                         m_AddError("No 'FROM' keyword found");
                         return response;
                     }
               }
               
               std::string tableName = "";
               try
               {
                   tableName = words.at(FROM+1);
               }
               catch(std::out_of_range)
               {
                   m_AddError("No table name specified");
                   response.code = SQL_SYNTAX_ERROR;
                   return response;
               }
               if(columnNamesSize == 1 && columnNames[0] == "*")
               {
                   SqlReturn = m_GetAllRows(tableName);
                   SqlReturnLength = m_GetRowCount(tableName);
                   response.code = SQL_OK;
                   return response;
               }
               if(tableName.at(tableName.size()-1) == ';')
               {
                   tableName.erase(tableName.size()-1,1);
               }
               bool usingWhere = 1;
               try
               {
                  std::string strWHERE = words.at(FROM+2);
               }
               catch(std::out_of_range)
               {
                 usingWhere = 0;  
               }
               if(usingWhere)
               {
                   //TODO handle WHERE
                   response.code = SQL_NOT_IMPLEMENTED;
               }
               else
               {
                   std::vector<DBRow> tmpRows = std::vector<DBRow>();
                   int tmpresultSize = 0;
                   DBRow* _rows = GetAllRowsFromTable(tableName,tmpresultSize);
                   if(_rows != notable && _rows != norow)
                   {
                   for(int z = 0; z < tmpresultSize;z++)
                   {
                       DBRow r = DBRow();
                       for(int a = 0; a < columnNamesSize; a++)
                       {
                        r.InsertData(_rows[z].Find(columnNames[a]));
                       }
                       tmpRows.push_back(r);
                   }
                   delete[] _rows;
                   SqlReturn = m_ArrayFromVector(tmpRows,SqlReturnLength);
                   response.code = SQL_OK;
                   return response;
                   }
                   else
                   {
                       std::string err = "Could not find table: " + tableName;
                       m_AddError(err);
                       response.code = SQL_NO_TABLE;
                       return response;
                   }
               }
               
           }
           else if(words.at(0) == "update")
           {
               CommandKeys.push_back(SQLCommand::UPDATE);
           }
           else if(words.at(0) == "alter")
           {
               CommandKeys.push_back(SQLCommand::ALTER);
           }
           else if(words.at(0) == "create")
           {
               StrToLower(words.at(1));
               CommandKeys.push_back(SQLCommand::CREATE);
               if(words.at(1) == "table")
               {
                   AffectedEntities.push_back(SQLEntity::TABLE);
                   std::string tableName = "";
                   if(words.at(2).find("(") != std::string::npos)
                   {
                       //Is the "(" the last character?
                       if(words.at(2).find("(") == words.at(2).length()-1)
                       {
                        words.insert(words.begin()+3,"(");
                        words.at(2) = words.at(2).substr(0,words.at(2).find("("));
                        
                       }
                       else
                       {
                           size_t pos = words.at(2).find("(");
                           words.insert(words.begin()+3,"(");
                           words.insert(words.begin()+4,words.at(2).substr(pos+1,(words.at(2).length()-1)-pos));
                           words.at(2) = words.at(2).substr(0,words.at(2).find("("));
                       }
                       
                   }
                   tableName = words.at(2);
                   if(verbose)
                   {
                     std::cout << TERMINAL_CYAN << "[Info] {SQL Code Processing} Guessing action to be: create a new table with name: " << tableName << std::endl << "[Info] {SQL Code Processing} Starting Column Detection..." << TERMINAL_NOCOLOR << std::endl;  
                     
                   }
                   std::string columns = "";
                   for(int u = 3; u < words.size();u++)
                   {
                       columns+=(words.at(u)+" ");
                   }
                   if(verbose)
                   {
                     std::cout << TERMINAL_CYAN << "[Info] {SQL Code Processing} Looking for column clues in: " << columns << TERMINAL_NOCOLOR << std::endl;
                   }
                   int c_pos = columns.find(",");
                   int count = 0;
                   if(c_pos != std::string::npos)
                   {
                        count++;
                        c_pos++;
                   }
                   while(columns.find(",",c_pos) != std::string::npos)
                   {
                       count++;
                       c_pos = columns.find(",",c_pos)+1;
                   }
                   c_pos = 0;
                   std::vector<DBColumn> dbColumns = std::vector<DBColumn>();
                   std::vector<DataType> columnTypes = std::vector<DataType>();
                   for(int c = 0; c < count+1;c++)
                   {
                      DBColumn tmpColumn = DBColumn();
                      int start = 0;
                      if(c == 0)
                      {
                          start = columns.find("(")+1;
                          if(columns.at(start) == ' ')
                          {
                              start++;
                          }
                      }
                      else
                      {
                          start = c_pos;
                      }
                      int end = columns.find(" ",start);
                      
                      std::string tmpName = columns.substr(start,end-start);
                      if(tmpName.find("PRIMARY") != std::string::npos)
                      {
                          std::string tmpPrim = columns.substr(columns.find("(",start)+1,columns.find(")",start)-(columns.find("(",start)+1));
                          bool found = 0;
                          for(int z = 0; z < dbColumns.size();z++)
                          {
                             if(dbColumns.at(z).name == tmpPrim)
                             {
                                 if(verbose)
                                 {
                                   std::cout << TERMINAL_CYAN << "[Info] {SQL Code Processing} Setting PRIMARY KEY to column: " << tmpPrim << TERMINAL_NOCOLOR << std::endl;  
                                 }
                                 dbColumns.at(z).settingsByte = (dbColumns.at(z).settingsByte | ColumnSettings::PRIMARY_KEY);
                             }
                             found = 1;
                             break;
                          }
                          if(!found)
                          {
                              response.code = SQL_SYNTAX_ERROR;
                              return response;
                          }
                          c_pos = columns.find(")",start)+1;
                          continue;
                      }
                      tmpColumn.name = tmpName;
                      start = end+1;
                      end = columns.find(",",start);
                      std::string tmpTypeString = columns.substr(start,end-start);
                      if(tmpTypeString.find(")")+1 != tmpTypeString.length())
                      {
                          int index = tmpTypeString.find(")");
                          std::string cmpString = "";
                          for(int j = 0; j < tmpTypeString.length()-index;j++)
                          {
                            cmpString+=" ";
                          }
                          std::string settingsStr = tmpTypeString.substr(index+1,tmpTypeString.length()-index);
                          if(settingsStr != cmpString)
                          {
                              if(verbose)
                              {
                                std::cout << TERMINAL_CYAN << "[Info] {SQL Code Processing} Possible Settings for Column: " << tmpColumn.name << TERMINAL_NOCOLOR << std::endl;
                              }
                              settingsStr.erase(remove_if(settingsStr.begin(), settingsStr.end(), isspace), settingsStr.end());
                              if(settingsStr == "NOTNULL")
                              {
                                  tmpColumn.settingsByte = tmpColumn.settingsByte | ColumnSettings::NOT_NULL;
                              }
                              else
                              {
                                 if(verbose)
                                {
                                    std::cout << TERMINAL_CYAN << "[Info] {SQL Code Processing} Unknown or unimplemented settings for column: " << '"' << tmpColumn.name << '"' << ". Setting that caused the error: " << settingsStr << TERMINAL_NOCOLOR << std::endl;
                                }
                                 response.code = SQL_SYNTAX_ERROR;
                                 return response;  
                              }
                          }
                      }
                      if(tmpTypeString.find("(") != std::string::npos)
                      {
                          std::string tmpTypeStringNoSize = tmpTypeString.substr(0,tmpTypeString.find("("));
                          std::transform(tmpTypeStringNoSize.begin(), tmpTypeStringNoSize.end(), tmpTypeStringNoSize.begin(), ::tolower);
                          if(tmpTypeStringNoSize == "int")
                          {
                              tmpColumn.type = DataType::INT;
                          }
                          else if (tmpTypeStringNoSize == "varchar")
                          {
                              tmpColumn.type = DataType::VARCHAR;
                          }
                          else
                          {
                            response.code = SQL_NOT_IMPLEMENTED;
                            return response;
                          }
                          int tmpStart = tmpTypeString.find("(")+1;
                          int tmpEnd = tmpTypeString.find(")");
                          if(tmpEnd == std::string::npos)
                          {
                            response.code = SQL_SYNTAX_ERROR;
                            return response;
                          }
                          std::string columnSize = tmpTypeString.substr(tmpStart,tmpEnd-tmpStart);
                          tmpColumn.size = std::stoi(columnSize);
                      }
                      else
                      {
                          std::transform(tmpTypeString.begin(), tmpTypeString.end(), tmpTypeString.begin(), ::tolower);
                          if(tmpTypeString == "int")
                          {
                              tmpColumn.type = DataType::INT;
                              tmpColumn.size = 8;
                          }
                          else if (tmpTypeString == "varchar")
                          {
                              tmpColumn.type = DataType::VARCHAR;
                              tmpColumn.size = 256;
                          }
                          else
                          {
                            response.code = SQL_NOT_IMPLEMENTED;
                            return response;
                          }
                      }
                      c_pos = end+2;
                      dbColumns.push_back(tmpColumn);
                      
                   }
                   uint64_t vectorSize = 0;
                   DBColumn* arr = m_ArrayFromVector(dbColumns,vectorSize);
                   CreateTable(tableName,vectorSize,arr);
                   response.code = SQLResponseCode::SQL_OK;
                   delete[] arr;
                   
                    
               }
               else if(words.at(1) == "index") //Indexing on columns are not easy to make T_T
               {
                   AffectedEntities.push_back(SQLEntity::INDEX);
                   response.code = SQL_NOT_IMPLEMENTED;
                   break;
               }
               else
               {
                   response.code = SQLResponseCode::SQL_SCRIPT_ERROR;
                   if(verbose)
                   {
                    std::cout << "[Fatal] {SQL Code Processing} Can't understand SQL Entity: " << words.at(1) << std::endl;
                    break;
                   }
               }
               
               
           }
           else if(words.at(0) == "delete")
           {
               CommandKeys.push_back(SQLCommand::DELETE);
               
           }
           else if(words.at(0) == "insert")
           {
               CommandKeys.push_back(SQLCommand::INSERT_INTO);
               
           }
           else
           {
               response.code = SQLResponseCode::SQL_SCRIPT_ERROR;
               if(verbose)
               {
                   std::cout << "[Fatal] {SQL Code Processing} Can't understand SQL command: " << words.at(0) << std::endl;
                   break;
               }
           }
           
       }
       
       
       return response;
   }
   DBRow* SqlReturn = nullptr;
   uint64_t SqlReturnLength = 0;
   #endif
   
   static DBRow* notable;
   static Key* nokey;
   static DBRow* norow;
   ~DBManager()
   {
       if(m_currDB != nullptr)
       {
           if(m_currDB->DBBuffer != nullptr)
           {
            delete (m_currDB);
           }
       }
   }
   
   
private:
    std::vector<std::string> m_errorVector = std::vector<std::string>();
    void m_AddError(std::string err)
    {
        m_errorVector.push_back(err);
    }
    
    bool BackUp()
    {
         if(m_currDB == nullptr)
         {
             std::cout << TERMINAL_RED << "Can't create table - No DB is loaded" << TERMINAL_NOCOLOR << std::endl;
             return 1;
         }
         std::ofstream out = std::ofstream("./backUp_dataBase",std::ios::binary);
         std::string tmpOut = "";
         if(m_currDB->DBSize > 2048)
         {
             //do some fancy stuff here - TEMPERARILY JUST DOES THE SAME AS IF THE FILE WAS SMALLER THAN 2048
             tmpOut.assign(UnsignedBuffer::ToSigned(m_currDB->DBBuffer, m_currDB->DBSize),m_currDB->DBSize);  
         }
         else
         {
           tmpOut.assign(UnsignedBuffer::ToSigned(m_currDB->DBBuffer, m_currDB->DBSize),m_currDB->DBSize);  
         }
         out.write(tmpOut.c_str(),tmpOut.size());
         out.close();
         return 0;
         
    }
    
    uint64_t m_GetRowOffsetByIndex(std::string tableName, uint32_t index)
    {
        if(m_GetRowCount(tableName)-1 > index)
        {
          return 0;  
        }
        uint64_t offset = m_GetLastColumnEndOffset(tableName);
        uint64_t EOT = m_GetEOT(tableName);
        int resultSize = 0;
        uint64_t* results = UnsignedString::Search(m_currDB->DBBuffer,offset,EOT,g_stoopidDBRowSig,6,resultSize);
        offset = results[index];
        delete[] results;
        return offset;
    }
    
    uint64_t* m_GetAllEntryOffsets(uint32_t &returnSize) //Returns 0xFFFFFFFFFFFFFFFF on error
    {
        uint64_t eTablePtr = GetEntryPointer();
        int32_t resultSize = 0;
        uint64_t* results = UnsignedString::Search(m_currDB->DBBuffer,eTablePtr,m_currDB->DBSize, g_stoopidDBTableSig,8,resultSize);
        
        if(resultSize == 0)
        {
          m_AddError("No EntryTableSignature?");
          return nullptr;
        }
        uint64_t end = results[0];
        delete[] results;
        
        results = UnsignedString::Search(m_currDB->DBBuffer,eTablePtr,end,g_stoopidDBEntrySig,6,resultSize);
        returnSize = static_cast<uint32_t>(resultSize);
        return results;
    }
    
    uint32_t m_GetTableCount() //Returns 0xFFFFFFFF on error
    {
        uint32_t size = 0;
        uint64_t* offsets = m_GetAllEntryOffsets(size);
        delete[] offsets;
        return size;
        
        
    }
    /*
     * @param Takes an offset *WITH THE SIGNATURE* and gets the length of the entry.
     * @return rowEntryLength.
     */
    uint64_t m_GetRowLength(uint64_t EntryWithSig)
    {
        uint64_t size = 0;
        for(int i = 0; i < 8;i++)
        {
           size = (size | m_currDB->DBBuffer[EntryWithSig+6+7-i]) << (i == 7 ? 0 : 8);
        }
        return size;
    }
    
    /*
     * 
     * @param Takes a table index and a offsetChange and modifies the requested table EOT, based on when it appears in the data - this index can be retrived through m_GetTableIndex() - Both this and m_GetTableIndex expects the entry table to work
     */
    void m_ModifyEOT(int tableIndex, int64_t offsetChange)
    {
      if(tableIndex == -1)
      {
        uint32_t tableCount = m_GetTableCount();
        uint32_t nameCount = 0;
        std::string* names = m_GetTableNames(nameCount);
        if(nameCount != tableCount)
        {
            m_AddError("[NOT FATAL] WARNING: m_GetTableCount and m_GetTableNames do not agree on the amount of tables. Be careful!");
        }
        for(uint32_t i = 0; i < tableCount; i++)
        {
            m_RewriteEOT(names[i],m_GetEOT(names[i])+offsetChange);
        }
      }
    }
    uint64_t m_GetNewRowWriteOffset(std::string tableName)
    {
        uint64_t rowsStart = m_GetLastColumnEndOffset(tableName);
        uint64_t rowsSize = m_GetEOT(tableName)-rowsStart;
        int rows = 0;
        uint64_t* offsets = UnsignedString::Search(m_currDB->DBBuffer,rowsStart,rowsSize,g_stoopidDBRowSig,6,rows);
        uint64_t returnOffset = m_GetLastColumnEndOffset(tableName);
        for(int i = 0; i < rows; i++)
        {
            returnOffset += m_GetRowLength(offsets[i]);
        }
        delete offsets;
        return returnOffset;
    }
    
    /*
     * @param Updates offsets in the entry table by reading tablenames without the help of the entrytable and checks if offsets are correct - and if not attempts to fix them - Useful after a resize operation
     */
    void m_FixEntryTable(int64_t expandSize)
    {
        uint32_t offsetsSize = 0;
        uint64_t* offsets = m_GetAllEntryOffsets(offsetsSize);
        uint32_t namesSize = 0;
        std::string* names = m_ScanForTableNames(namesSize);
        bool rebuild = 0;
        if(offsetsSize < namesSize)
        {
            m_AddError("[Non-Fatal] Entry table is missing entries - Full rebuild of entry table is recommended");
            rebuild = 1;
        }
        else if(namesSize < offsetsSize)
        {
            m_AddError("[Non-Fatal] Entry table has entries for deleted tables - Applies workaround (full table rebuild)");
            rebuild = 1;
            
        }
        if(!rebuild)
        {
            for(uint32_t i = 0; i < offsetsSize;i++)
            {
              uint64_t offset = 0;
              std::string tmp_name = m_ReadString(offsets[i]+6+4,255);
              for(int u = 0; u < 8; u++)
              {
                offset = (offset | m_currDB->DBBuffer[offsets[i]+6+4+tmp_name.length()+1+7-u]) << ( u == 7 ? 0 : 8);
              }
              if(m_ReadString(offset,255) != names[i])
              {
                  std::cout << "[Non-Fatal] Wrong entry pointer - Attempting to quick fix offset" << std::endl;
                  offset += expandSize;
                  if(m_ReadString(offset,255) != names[i])
                  {
                      std::cout << "[Non-Fatal] Entry pointer completly lost - Rebuiling entry" << std::endl;
                      uint32_t size = 0;
                      uint64_t* sets = m_ScanForTableNameOffsets(size);
                      offset = sets[i];
                      delete[] sets;
                  }
              }     
              for(int u = 0; u < 8; u++)
              {
                //m_currDB->DBBuffer[offsets[i]+6+4+tmp_name.length()+1+7-u] = (uchar)((uchar*)&offset)[u]; //< -- This is the method used for reading the data not writing it - as this writes in the big-endian, which makes the little endian reader fucked
                m_currDB->DBBuffer[offsets[i]+6+4+tmp_name.length()+1+u] = (uchar)((uchar*)&offset)[u]; //<-- This is a fine little endian writer.
              }
            }
        }
        else
        {
            //Setting rebuild by overwriting the offsets array with a full scan versionString
            delete[] offsets;
            offsets = m_ScanForTableNameOffsets(offsetsSize);
            //Let's just assume that at least the first entry in the table exists.
            int tableOffsetSize = 0;
            uint64_t* tableOffsetArray = UnsignedString::Search(m_currDB->DBBuffer,offsets[offsetsSize-1],m_currDB->DBSize, g_stoopidDBEntrySig,6,tableOffsetSize);
            uint64_t newTableOffset = tableOffsetArray[0];
            delete[] tableOffsetArray;
            m_CreateNewEntryTable(newTableOffset);
        }
        delete[] names;
        delete[] offsets;
        
        
    }
    
    std::string* m_GetTableNames(uint32_t &arraySize)//**NOT** A SCAN FUNCTION - So we expect the table to work
    {
        uint64_t* entOffsets = m_GetAllEntryOffsets(arraySize);
        std::string* names = new std::string[arraySize];
        for(uint32_t i = 0; i < arraySize;i++)
        {
            names[i] = m_GetTableNameFromEntry(entOffsets[i]);
        }
        return names;
        
        
        
    } 
    
    /*
     * @param Takes a an entry **WITH** signature and returns the name of the table from the entry
     */
    std::string m_GetTableNameFromEntry(uint64_t entry)
    {
        return m_ReadString(entry+6+4,255);
    }
    
        /*
     *@param Scans for names of tables without using the entry table - Useful if you suspect the entry table to be corrupted - However it does depend on the size of the tables being recorded acurately
     *
     */
    std::string* m_ScanForTableNames(uint32_t &arraySize)
    {
       //uint32_t count = m_GetTableCount(); // AGAIN "SCAN"-FUNCTIONS SHOULD **NEVER** USE THE FUCKING ENTRYTABLE AS SCAN EXPECTS IT TO BE FUCKED!!!
       uint32_t count = m_ScanForTableCount(); //<<-- Do this
       arraySize = count;
       uint64_t offset = 0;
       std::string* arr = new std::string[count]; 
       for(uint32_t i = 0; i < count; i++)
       {
           if(i == 0)
           {
               offset = 12;
           }
           arr[i] = m_ReadString(offset,255);
           offset += arr[i].length()+1;
           offset = m_ReadUnsingedValue(8,offset);
           
       }
       return arr;
       
    }
    
    void m_CreateNewEntryTable(uint64_t offset)
    {
        uint64_t currOffset = offset;
        uint32_t namesSize = 0;
        std::string* names = m_ScanForTableNames(namesSize);
        std::cout << "Found names: " << std::endl << "[START]" << std::endl;
        for(uint32_t i = 0; i < namesSize;i++)
        {
            std::cout << names[i] << std::endl;
        }
        std::cout << "[END]" << std::endl;
        uint32_t expectedSize = 0;
        for(uint32_t i = 0; i < namesSize; i++)
        {
            expectedSize+=names[i].length()+1+8+6+4; //+1 for Null terminator, +4 for entry length, +8 for offset and +6 for ENT signature
        }
        expectedSize+=8+8; //+8 For end signature and +8 for last 8 bytes of file
        if(offset+expectedSize > m_currDB->DBSize)
        {
            ResizeDB(offset+expectedSize,1);
        }
        uint32_t offsetsSize = 0;
        uint64_t* offsets = m_ScanForTableNameOffsets(offsetsSize);
        for(uint32_t i = 0; i < offsetsSize;i++)
        {
            m_DirectWrite(currOffset,6,g_stoopidDBEntrySig);
            currOffset+=6;
            uint32_t entSize = names[i].length()+1+6+8+4;
            m_DirectWrite(currOffset,4,(uchar*)&entSize); //+1 for nullterminator, +6 for signature, and +8 for offset
            currOffset+=4; //+4 for entry size
            m_DirectWriteNullFill(currOffset,names[i].length(),names[i].length()+1,names[i].c_str());
            currOffset+=names[i].length()+1;
            m_DirectWrite(currOffset,8,(uchar*)&offsets[i]);
            currOffset+=8;
        }
        m_DirectWrite(currOffset,8,g_stoopidDBTableSig);
        currOffset+=8;
        ResizeDB(currOffset+8,1);
        delete[] offsets;
        delete[] names;
        m_DirectWrite(m_currDB->DBSize-8,8,(uchar*)&offset);
        m_WriteDBToDisk();
        std::cout << "Created new table" << std::endl;
    }
    
    uint64_t* m_ScanForTableNameOffsets(uint32_t &arraySize)
    {
       //uint32_t count = m_GetTableCount(); //Relies on m_GetEntryPointer which is not reliable in SCAN functions as SCAN functions are meant to be used when the table is fucked
       uint32_t count = m_ScanForTableCount(); //<<-- Do this
       arraySize = count;
       uint64_t offset = 0;
       uint64_t* arr = new uint64_t[count]; 
       for(uint32_t i = 0; i < count; i++)
       {
           if(i == 0)
           {
               offset = 12;
           }
           arr[i] = offset;
           std::string tmp = m_ReadString(offset,255);
           offset += tmp.length()+1;
           offset = m_ReadUnsingedValue(8,offset);
           
       }
       return arr;
    }
    
    DBRow* m_GetRowByPrimaryKey(std::string tableName, std::string primKeyValue)
    {
        uint32_t rows = m_GetRowCount(tableName);
        std::string primKeyName = m_GetPrimaryKey(tableName);
        if(primKeyName == "")
        {
            return 0x0; //Error equal to nullptr
        }
        DBRow* dbRows = m_GetAllRows(tableName);
        for(uint32_t i = 0; i < rows; i++)
        {
            for(uint64_t u = 0; i < dbRows[i].keys.size();u++)
            {
            if(dbRows[i].keys.at(u).name == primKeyName)
            {
                if(dbRows[i].keys.at(u).value == primKeyValue)
                {
                    DBRow* r = new DBRow(dbRows[i]);
                    delete[] dbRows;
                    return r;
                }
            }
            }
        }
        delete[] dbRows;
        return norow;
        
    }
    
    std::string m_GetPrimaryKey(std::string tableName)
    {
        uint64_t offset = m_GetColumnsStart(tableName);
        uint32_t columnCount = m_GetColumnCount(tableName);
        std::string* names = m_GetAllColumnNames(tableName);
        for(uint32_t i = 0; i < columnCount; i++)
        {
            UnsignedString tmp = m_ReadUnsignedString(offset,255,1);
            uint64_t columnSize = m_ReadUnsingedValue(4, offset+tmp.length());
            uint64_t nextOffset = offset+columnSize;
            uchar settingsByte = m_ReadUnsingedValue(1,offset+tmp.length()+4+1);
            uchar binOpResult = settingsByte & (uchar)0b00100000;
            if(binOpResult == (uint)0b100000)
            {
                std::string name = names[i];
                delete[] names;
                return name;
            }
            offset = nextOffset;
        }
        std::string tmpS = "[Warning] No primary key in table: ";
        tmpS += tableName;
        m_AddError(tmpS);
        delete[] names;
        return "";
        
        
    }
    
    uint32_t m_ScanForTableCount()//ONLY USE WHEN THE TABLE IS **COMPLETlY** FUCKED AS THIS WASTES SOOOOOO MUCH PROCESSING POWER
    {
        uint32_t count = 0;
        for(uint64_t i = 12; i < m_currDB->DBSize;i+=0)
        {
            i = m_ReadUnsingedValue(8,i+m_ReadUnsignedString(i,255).length()); //TODO Make sure that we don't find empty or made up tables
            if(i < m_currDB->DBSize){ count++;}
        }
        return count;
    }
    
    DBRow* m_GetAllRows(std::string tableName, bool ignoreNewest=false)
    {
        uint32_t rowCount = m_GetRowCount(tableName);
        DBRow* rows = new DBRow[rowCount];
        uint64_t offset = m_GetLastColumnEndOffset(tableName);
        uint64_t EOT = m_GetEOT(tableName);
        int resultSize = 0;
        uint64_t* results = UnsignedString::Search(m_currDB->DBBuffer,offset,EOT,g_stoopidDBRowSig,6,resultSize);
        for(uint32_t i = 0; i < (ignoreNewest==true ? rowCount-1 : rowCount); i++)
        {
            uint64_t rowOffset = 0;
            uint64_t rowLength = m_GetRowLength(results[i]);
            while(true)
            {
                std::string columnName = m_GetColumnNameFromID(tableName,m_ReadUnsingedValue(4,results[i]+6+8+rowOffset));
                uint32_t columnMAX = m_GetColumnDataMaxSize(tableName,columnName);
                DataType type = DataType(m_GetColumnType(tableName,columnName));
                std::string dataString = "";
                switch(type)
                {
                    case DataType::INT:{
                        UnsignedString uData = m_ReadUnsignedString(results[i]+6+8+4+rowOffset,columnMAX,0);
                        uint32_t tmp = 0;
                        if(columnMAX <= 8)
                        {
                        for(uint32_t u = 0; u < columnMAX; u++)
                        {
                            
                            tmp = (tmp | uData.at(columnMAX-1-u)) << (u == columnMAX-1 ? 0 : 8);
                        }
                        dataString = std::to_string(static_cast<int64_t>(tmp));
                        break;
                        }
                        else
                        {
                            //The number is larger than 64 bits - a solution is needed for this
                          break;  
                        }
                    }
                    
                    case DataType::VARCHAR:{
                        UnsignedString uData = m_ReadUnsignedString(results[i]+6+8+4+rowOffset,columnMAX,1);
                        dataString = uData.ToSigned();
                    break;
                    }
                    case DataType::TIME_DATE:{
                        //TODO
                    break;
                    }
                    default:
                    {
                        std::string tmpS = "[Non-Fatal] Could Not Get DataType of Column: ";
                        tmpS+="<" + tableName + "," + columnName + ">";
                        m_AddError(tmpS);
                    break;
                    }
                }
                rows[i].InsertData(m_GetColumnNameFromID(tableName,m_ReadUnsingedValue(4,results[i]+6+8+rowOffset)),dataString);
                rowOffset += columnMAX+4;
                if(results[i]+6+8+4+rowOffset > results[i]+rowLength)
                {
                    break;
                }
                
            }
        }
        delete[] results;
        return rows;
    }
    
    std::string m_GetColumnNameFromID(std::string tableName, uint32_t id)
    {
        std::string name = "";
        std::string* names = m_GetAllColumnNames(tableName);
        name = names[id];
        delete[] names;
        return name;
        
    }
    
    uchar m_GetColumnSettings(std::string tableName, std::string columnName)
    {
       uint64_t offset =  m_GetColumnOffset(tableName,columnName);
       UnsignedString utmp = m_ReadUnsignedString(offset,255);
       uchar settings = m_ReadUnsingedValue(1,offset+utmp.length()+4+1);
       if(settings == 0)
       {
         std::string tmp = "[Non-Fatal] Column Settings for " + m_ReadString(offset,255);
         tmp += " are invalid";
         m_AddError(tmp);
       }
       return settings;
    }
    
    template<typename T>
    std::vector<T> m_VectorFromArray(T* arr, uint64_t len)
    {
        std::vector<T> tmpVec = std::vector<T>();
        for(uint64_t i = 0; i < len; i++)
        {
            tmpVec.push_back(arr[i]);
        }
        return tmpVec;
    }
    template<typename T>
    T* m_ArrayFromVector(std::vector<T> vec, uint64_t &arrSize)
    {
        T* arr = new T[vec.size()];
        for(uint64_t i = 0; i < vec.size();i++)
        {
            arr[i] = vec.at(i);
        }
        arrSize = vec.size();
        return arr;
    }
    
    
    uchar m_GetColumnType(std::string tableName, std::string columnName)
    {
       return m_ReadUnsingedValue(1,m_GetColumnOffset(tableName,columnName)+columnName.length()+1+4);  
    }
    
    bool m_WriteAutoAndUpdateOffset(std::string tableName, uint64_t &offset)
    {
        uint32_t rowCount = m_GetRowCount(tableName);
        std::string primKey = m_GetPrimaryKey(tableName);
        if(primKey == "")
        {
          m_AddError("[Error Non-Fatal] No PrimkeyColumn Found");
          return false;  
        }
        
        uint64_t currAutoInc = 0xFFFFFFFFFFFFFFFF;
        if(rowCount-1 > 0)
        {
            DBRow* rows = m_GetAllRows(tableName,1);
            DBRow r = rows[rowCount-1-1];
            if(primKey != "" && rows[rowCount-1-1].Find(primKey) != *nokey)
            {
                currAutoInc = std::stoi(rows[rowCount-1-1].Find(primKey).value);
            }
        }
        
        if(m_GetColumnType(tableName,primKey) != 0)
        {
            m_AddError("[Error] Cannot auto incement a non-int value");
            return false;
        }
        
        currAutoInc++;
        uint32_t id = m_GetColumnID(tableName,primKey);
        if(id != 0xFFFFFFFF)
        {
        m_DirectWrite(offset,4,(uchar*)&id);
        m_DirectWriteNullFill(offset+4,8,m_GetColumnDataMaxSize(tableName,primKey),(uchar*)&currAutoInc);
        offset+=4+m_GetColumnDataMaxSize(tableName,primKey);
        return true;
        }
        else
        {
            m_AddError("[ERROR] Couldn't get ID of table: " + primKey);
            return false;
        }
    }
    
    uint64_t m_CalculateSpaceForUnassignedColumns(std::string tableName, std::vector<Key> assignedColumns)
    {
        
        uint64_t extraSize = 0;
        
        //For all columns marked as NOT NULL
        uint32_t columnCount = m_GetColumnCount(tableName);
        std::string* names = m_GetAllColumnNames(tableName);
        
        std::vector<std::string> tmpVec = m_VectorFromArray<std::string>(names, columnCount);
        
        for(uint32_t i = 0; i < assignedColumns.size(); i++)
        {
            std::string name = assignedColumns.at(i).name;
            for(uint32_t u = 0; u < tmpVec.size();u++)
            {
                if(tmpVec.at(u) == name)
                {
                    tmpVec.at(u) = "";
                }
            }
            
        }
        for(uint32_t i = 0; i < tmpVec.size();i++)
        {
            if(tmpVec.at(i) != "")
            {
                 if(((m_GetColumnSettings(tableName, tmpVec.at(i)) & (uchar)0b10000000) == (uchar)128) || ((m_GetColumnSettings(tableName, tmpVec.at(i)) & (uchar)0b01000000) == (uchar)64))
                 {
                     extraSize += m_GetColumnDataMaxSize(tableName,tmpVec.at(i));
                     extraSize += 4; //For ID
                 }
            }
        }
        
       
        
        delete[] names;
        tmpVec.clear();
        return extraSize;
        
    }
    
    uint64_t m_ReadUnsingedValue(uint bytesToRead, uint64_t offset)
    {
        uint64_t data = 0;
        for(uint i = 0; i < bytesToRead;i++)
        {
            data = (data | m_currDB->DBBuffer[offset+bytesToRead-1-i]) << (i == bytesToRead-1 ? 0: 8);
        }
        return data;
    }
    
    uint32_t m_GetRowCount(std::string tableName)
    {
        uint64_t offset = m_GetLastColumnEndOffset(tableName);
        uint64_t EOT = m_GetEOT(tableName);
        int resultSize = 0;
        uint64_t* results = UnsignedString::Search(m_currDB->DBBuffer,offset,EOT,UnsignedString(g_stoopidDBRowSig,6),resultSize);
        delete[] results;
        return static_cast<uint32_t>(resultSize);
    }
    
    void m_NullFill(uint64_t start, uint64_t length)
    {
        for(uint64_t i = 0; i < length; i++)
        {
            m_currDB->DBBuffer[start+i] = static_cast<uchar>(0x0);
        }
    }
    
    void m_UpdateEntryTableOffset(int64_t sizeChange)//Yes it's a signed int - to make sure that we can pass negative values if we make the DB smaller
    {
       uint64_t oldEtablePtr = GetEntryPointer();
       uint64_t newEtablePtr = oldEtablePtr+static_cast<uint64_t>(sizeChange);
       uchar* p = (uchar*)&newEtablePtr;
       for(int i = 0; i < 8;i++)
       {         
            m_currDB->DBBuffer[m_currDB->DBSize-8+i] = p[i];
       }
    }
    
    uint32_t m_GetColumnID(std::string tableName, std::string columnName) //Expects that the table is valid
    {
        uint32_t id = 0xFFFFFFFF;
        uint64_t offset = m_GetColumnOffset(tableName, columnName);
        uint32_t columnCount = m_GetColumnCount(tableName);
        std::string* columns = m_GetAllColumnNames(tableName);
        if(offset != 0)
        {
            
            for(uint32_t i = 0; i < columnCount;i++)
            {
                if(columns[i] == columnName)
                {
                  id = i;
                  break;
                }
            }
            
        }
        
        return id;
        
    }
    
    void m_WriteDBToDisk()
    {
        if(m_currDB->DBSize < (1048576*2))
        {
        std::ofstream out = std::ofstream(m_currDB->path, std::ios::binary);
        std::string s_out = "";
        s_out.assign(UnsignedBuffer::ToSigned(m_currDB->DBBuffer,m_currDB->DBSize),m_currDB->DBSize);
        out.write(s_out.c_str(),m_currDB->DBSize);
        out.close();
        }
        else //prevent creating a large copy of the buffer if the buffer is larger than 2MiB
        {
            std::ofstream out = std::ofstream(m_currDB->path, std::ios::binary);
            for(uint64_t i = 0; i < m_currDB->DBSize;i++)
            {
                out << static_cast<char>(m_currDB->DBBuffer[i]);
            }
            out.close();
        }
    }
    
    void m_DirectWriteNullFill(uint64_t offset, uint64_t realLength ,uint64_t nullFilledLength, uchar* dataToWrite)
    {
        m_DirectWrite(offset,realLength,dataToWrite);
        for(uint64_t i = 0; i < nullFilledLength-realLength;i++)
        {
           m_currDB->DBBuffer[offset+realLength+i] = static_cast<uchar>('\0'); 
        }
    }
    void m_DirectWriteNullFill(uint64_t offset, uint64_t realLength ,uint64_t nullFilledLength, const char* dataToWrite)
    {
        uchar* data = new uchar[realLength];
        for(uint64_t i = 0; i < realLength;i++)
        {
            data[i] = static_cast<uchar>(dataToWrite[i]);
        }
        m_DirectWrite(offset,realLength,data);
        for(uint64_t i = 0; i < nullFilledLength-realLength;i++)
        {
           m_currDB->DBBuffer[offset+realLength+i] = static_cast<uchar>('\0'); 
        }
        delete[] data;
    }
    
    void m_DirectWrite(uint64_t offset, uint64_t length, uchar* dataToWrite)
    {
        for(uint64_t i = 0; i < length; i++)
        {
            m_currDB->DBBuffer[offset+i] = dataToWrite[i];
        }
    }
    
    void m_PushData(uint64_t from_offset, uint64_t push_length)
    {
        for(uint64_t i = 0; i < m_currDB->DBSize-from_offset-push_length; i++)
        {
            //m_currDB->DBBuffer[m_currDB->DBSize-1-i] = m_currDB->DBBuffer[from_offset+push_length-i];
            m_currDB->DBBuffer[m_currDB->DBSize-1-i] = m_currDB->DBBuffer[m_currDB->DBSize-1-push_length-i];
        }
    }
    void m_PullData(uint64_t offset, uint64_t pull_length)
    {
        for(uint64_t i = 0; i < m_currDB->DBSize-offset-pull_length; i++)
        {       
            //m_currDB->DBBuffer[m_currDB->DBSize-1-i] = m_currDB->DBBuffer[m_currDB->DBSize-1-pull_length-i];
            m_currDB->DBBuffer[offset+i] = m_currDB->DBBuffer[offset+pull_length+i];
            
        }
    }
    
    uint64_t m_GetEOT(std::string tableName)
    {
        uint64_t offset = TableExist(tableName);
        uint64_t len = 0;
        std::string tmp =  m_ReadString(offset,255);
        offset += tmp.length()+1;//+1 for nullbyte
        for(int i = 0; i < 8; i++)
        {
            len = (len | m_currDB->DBBuffer[offset+7-i]) << (i == 7 ? 0 : 8);
        }
        return len;
        
    }
    uint64_t m_GetEOT(uint64_t _offset)
    {
        uint64_t offset = _offset;
        uint64_t len = 0;
        std::string tmp = m_ReadString(offset,255);
        offset += tmp.length()+1; //+1 for nullbyte
        for(int i = 0; i < 8; i++)
        {
            len = (len | m_currDB->DBBuffer[offset+7-i]) << (i == 7 ? 0 : 8);
        }
        return len;
        
    }
    
    uint64_t m_GetColumnsStart(std::string tableName)
    {
         uint64_t offset = TableExist(tableName);
         std::string tmp_string = m_ReadString(offset,255);
         offset+= tmp_string.length()+1;//+1 for nullbyte terminator
         offset += 12;
         return offset;
         
    }
    
    std::string* m_GetAllColumnNames(std::string tableName)
    {
        uint64_t offset = m_GetColumnsStart(tableName);
        uint32_t columnCount = m_GetColumnCount(tableName);
        uint32_t columnCounter = 0;
        std::string* arr = new std::string[columnCount];
        
        while(true)
        {
            std::string cName = m_ReadString(offset, 255);
            arr[columnCounter] = cName;
                uint64_t tmp = offset+cName.length()+1;
                uint32_t ttmp = 0;
                for(int i = 0; i < 4; i++)
                {
                    ttmp = (ttmp | m_currDB->DBBuffer[tmp+3-i]) << (i == 3 ? 0 : 8);
                }
                offset+=ttmp;
                columnCounter++;
            if(offset > m_GetLastColumnEndOffset(tableName) || columnCounter >= columnCount)
            {
                break;
            }
        }
        return arr;
    }
    
    uint32_t m_GetColumnCount(std::string tableName)
    {
        uint64_t offset = TableExist(tableName);
        std::string tmp_string = m_ReadString(offset,255);
        offset+=tmp_string.length()+1; //+1 for nullbyte terminator
        offset+=8; //for EOT
        uint32_t columns = 0;
        for(int i = 0; i < 4; i++)
        {
           columns = (columns | m_currDB->DBBuffer[offset+3-i]) << (i == 3 ? 0 : 8); 
        }
        return columns;
    }
    
    uint64_t m_GetColumnOffset(std::string tableName, std::string columnName)
    {
        uint64_t offset = m_GetColumnsStart(tableName);
        uint32_t size = 0;
        uint32_t columns = m_GetColumnCount(tableName);
        uint32_t columnCounter = 0;
        while(true)
        {
            std::string cName = m_ReadString(offset, 255);
            if(cName == columnName)
            {
                
                break;
            }
            else
            {
                uint64_t tmp = offset+cName.length()+1;
                uint32_t ttmp = 0;
                for(int i = 0; i < 4; i++)
                {
                    ttmp = (ttmp | m_currDB->DBBuffer[tmp+3-i]) << (i == 3 ? 0 : 8);
                }
                offset+=ttmp;
                columnCounter++;
                
            }
            if(offset > m_GetLastColumnEndOffset(tableName) || columnCounter > columns)
            {
                offset = 0;
                break;
            }
        }
        return offset; //0 == not found
    }
    
    uint32_t m_GetColumnDataMaxSize(std::string tableName, std::string columnName)
    {
                uint32_t size = 0;
                uint64_t correctColumnOffset = m_GetColumnOffset(tableName, columnName);
                if(correctColumnOffset != 0)
                {
                correctColumnOffset += columnName.length()+1+4+1+1;
                
                for(int i = 0; i < 4; i++)
                {
                   size = (size | m_currDB->DBBuffer[correctColumnOffset+3-i]) << (i == 3 ? 0 : 8); 
                }
                return size; 
                }
                return 0; //0 == not found
        
       
    }
    /*
     Gets the offset of the last byte in the list of all the columns of the current table (ONLY EQUAL TO EOT IF THERE IS NO ROWS IN THE TABLE!)
     */
    uint64_t m_GetLastColumnEndOffset(std::string tableName)
    {
        
        uint64_t offset = m_GetColumnsStart(tableName);
        uint32_t columns = m_GetColumnCount(tableName);
        uint32_t columnCounter = 0;
        while(columnCounter < columns)
        {
             std::string column_tmp_name = m_ReadString(offset,255);
             uint32_t columnSize = 0;
             for(int i = 0; i < 4; i++)
             {
                 columnSize = (columnSize | m_currDB->DBBuffer[offset+column_tmp_name.length()+1+3-i]) << (i == 3 ? 0 : 8); //+1 for nullbyte terminator
             }
             offset += columnSize;
             columnCounter++;
        }
        return offset;
    }
    
    std::string m_ReadString(uint64_t _offset, uint maxRead)
    {
        std::string tmp = "";
        for(uint i = 0; i < maxRead;i++)
        {
            if(m_currDB->DBBuffer[_offset+i] == '\0')
            {
                break;
            }
            else
            {
                tmp += static_cast<char>(m_currDB->DBBuffer[_offset+i]);
            }
        }
        return tmp;
    }
    /*
     * @param Includes the null at the end of the string (so length() will return length WITH null)
     * If respectNullTerminator is set to false the buffer will NOT add a \0 byte at the end
     */
    UnsignedString m_ReadUnsignedString(uint64_t _offset, uint maxRead, bool respectNullTerminator=true)
    {
       std::string tmp = "";
        for(uint i = 0; i < maxRead;i++)
        {
            if((m_currDB->DBBuffer[_offset+i] == static_cast<uchar>('\0')) && (respectNullTerminator == true))
            {
                break;
            }
            else
            {
                tmp += static_cast<char>(m_currDB->DBBuffer[_offset+i]);
            }
        }
        UnsignedString nullStr = UnsignedString("");
        if(respectNullTerminator)
        {
            nullStr = UnsignedString(static_cast<uchar>('\0'));
        }
        UnsignedString utmp = UnsignedString(tmp);
        utmp += nullStr;
        return utmp; 
    }
    
    void ResizeDB(uint64_t newSize, bool echo=false)
    {
        if(echo)
        {
            std::cout << "Resizing DB from " << m_currDB->DBSize << " to " << newSize << std::endl;
        }
         m_currDB->DBBuffer = UnsignedBuffer::Resize(m_currDB->DBBuffer,m_currDB->DBSize,newSize);
         m_currDB->DBSize = newSize;
    }
    
    uint64_t GetEntryPointer()
    {
       if(m_currDB == nullptr)
       {
             std::cout << TERMINAL_RED << "Can't create table - No DB is loaded" << TERMINAL_NOCOLOR << std::endl;
             return 0;
       }
       uint64_t offset = 0;
       for(int i = 0; i < 8;i++)
       {
           //int mull = (8*i);
           //std::cout << "Read Byte : " << std::hex << std::setw(sizeof(uchar)*2) << "0x" << (int)m_currDB->DBBuffer[m_currDB->DBSize-i-1] << std::dec << "\0" << std::endl;
           offset = (offset | (uchar)m_currDB->DBBuffer[m_currDB->DBSize-i-1]) << ( i == 7 ? 0 : 8);
       }
       return offset;
    }
    
    void m_RewriteEOT(std::string tableName, uint64_t newEOT) //assumes that a check for the existance of the table has been done before
    {
        uint64_t offset = TableExist(tableName);
        std::string tmp_name = m_ReadString(offset,255);
        for(int i = 0; i < 8; i++)
        {
            m_currDB->DBBuffer[offset+tmp_name.length()+i+1] = (uchar)((uchar*)&newEOT)[i];
        }
        
    }
    void m_ForceRewriteEOT(uint64_t offset, uint64_t newEOT) //assumes that a check for the existance of the table has been done before
    {
        std::string tmp_name = m_ReadString(offset,255);
        for(int i = 0; i < 8; i++)
        {
            m_currDB->DBBuffer[offset+tmp_name.length()+i+1] = (uchar)((uchar*)&newEOT)[i];
        }
        
    }
    /*
     * @param Returns 0 on fail and the Offset on sucess - should always be tested against a "0" to make sure the system is working with a valid table
     */
    uint64_t TableExist(std::string name)
    {
       uint64_t Offset = 0;
       UnsignedString uname = UnsignedString(name);
       int resultSize = 0;
       
       uint64_t* results = UnsignedString::Search(m_currDB->DBBuffer, GetEntryPointer(), m_currDB->DBSize,g_stoopidDBTableSig,8,resultSize);
       if(resultSize == 0)
       {
           std::cout << TERMINAL_RED << "DB Corrupted!! - No EntryTableSignature found!!" << TERMINAL_NOCOLOR << std::endl;
           delete[] results;
           return 0;
       }
       uint64_t end = results[0];
       
       //result = UnsignedString::Search(m_currDB->DBBuffer, GetEntryPointer(), end, UnsignedBuffer(name.c_str(),name.length()).GetBuffer(), name.length(), resultSize);
       uint64_t* result = UnsignedString::Search(m_currDB->DBBuffer, GetEntryPointer(), end, uname, resultSize); //Sigh - The buffer that holds the name deletes it self in the middle if a while loop... WTF!!!!!!! - Fixed for some reason the overloaded "=" operator was used instead of the copyconstructor
       
       if(resultSize == 0)
       {
           delete[] results;
           delete[] result;
           return 0;
       }
       
       
       for(int i = 0; i < 8;i++)
       {
           //std::cout << "Read Byte : " << std::hex << std::setw(sizeof(uchar)*2) << "0x" << (int)(m_currDB->DBBuffer[result[0]+uname->length()+1+7-i]) << std::dec << "\0" << " offset " << result[0]+uname->length()+1+7-i << std::endl;
           //Offset = (Offset | m_currDB->DBBuffer[result[0]+uname->length()+1/*the +1 is for the nullbyte*/+i]) << (i == 7 ? 0 : 8);
           Offset = (Offset | m_currDB->DBBuffer[result[0]+uname.length()+1+7/*the +1 is for the nullbyte*/-i]) << (i == 7 ? 0 : 8);
       }
       
       delete[] results;
       delete[] result;
       return Offset;
       
       
    }
    
    void WriteEntryToTable(const std::string &name, uint64_t offset)
    {
        
    }
    std::string m_dbPath;
    DataBase* m_currDB = nullptr;

};

DBRow* DBManager::notable = new DBRow("NOT FOUND");
Key* DBManager::nokey = new Key("","");
DBRow* DBManager::norow = new DBRow("NO ROWS");
