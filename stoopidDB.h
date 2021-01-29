/*
 StoopidDB format Created and owned by AnzoDK (Anton F. Rosenørn-Dohn) 2021
 */
#pragma once
#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <streambuf>
#include <vector>
#include <unistd.h>

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


/*
 ChangeLog: Added copyconstructor for UnsignedBuffer - that for some reason accepted using a const UnsignedBuffer& as an argument, due to it having a overloaded operator= that gave it the power to create new UnsignedBuffers from others, without uisng the a copyconstructor. That problem seems to have been solved!!!
 
 */

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
    explicit UnsignedBuffer(size_t size)
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
    UnsignedBuffer(const char* s_buffer, int _length)
    {
        m_ucBuffer = new unsigned char[_length];
        
        length = _length;
        for(uint i = 0; i < length;i++)
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
    UnsignedBuffer(uchar* buffer, size_t _length)
    {
        length = _length;
        m_ucBuffer = new uchar[length];
        for(size_t i = 0; i < length;i++)
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
    void InsertAt(UnsignedBuffer buffer, size_t where)
    {
        for(size_t i = 0; i < buffer.length;i++)
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
        size_t c = 0;
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
    void Resize(uint newSize)
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
        for(size_t i = 0; i < str.length();i++)
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

    size_t length;
protected:
    unsigned char* m_ucBuffer;
};

class UnsignedString
{
public:
    
    int length()
    {
        size_t len = strBuffer.length;
        return len;
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
    UnsignedString(const uchar buff[], size_t size)
    {
        strBuffer = UnsignedBuffer(size);
        for(size_t i = 0; i < size; i++)
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
    static size_t* Search(uchar* buffer, size_t offset, size_t length, uchar* searchTerm, size_t searchTermLength, int &resultSize)
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
        size_t* indexes = new size_t[resultSize];
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
    static size_t* Search(uchar* buffer, size_t offset, size_t length, UnsignedString searchTerm, int &resultSize)
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
        size_t* indexes = new size_t[resultSize];
        
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
    static size_t* SearchFromEnd(uchar* buffer, size_t offset, uchar* searchTerm, size_t searchTermLength, int &resultSize)
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
        size_t* indexes = new size_t[resultSize];
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
    static size_t* SearchFromEnd(uchar* buffer, size_t offset, UnsignedString searchTerm, int &resultSize)
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
        size_t* indexes = new size_t[resultSize];
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
  Key Find(std::string name)
  {
     for(size_t i = 0; i < keys.size();i++)
     {
         if(keys.at(i).name == name)
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

struct DataBase
{

   std::string path;
   std::string versionString;
   unsigned int DBSize;
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
       uchar* tableSig = new uchar[8]{
           '\0',
           static_cast<uchar>('\xFF'),
           static_cast<uchar>('\xFF'),
           '\0',
           '\0',
           static_cast<uchar>('\xFF'),
           static_cast<uchar>('\xFF'),
           '\0'
           };
       uchar* entrySig = new uchar[6]{
           'E',
           static_cast<uchar>('\xFF'),
           'N',
           static_cast<uchar>('\xFF'),
           'T',
           static_cast<uchar>('\xFF')
           };
       
       while(true)
       {
           if(c+8 < m_currDB->DBSize-allowedWriteOffset)
           {
                if( UnsignedString(&m_currDB->DBBuffer[allowedWriteOffset+c],0,8) == UnsignedString(tableSig,0,8))
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
       uchar* tmpBuffer = new uchar[extraBuffSize];
       for(int u = 0; u < extraBuffSize;u++)
       {
         tmpBuffer[u] = m_currDB->DBBuffer[allowedWriteOffset+u];
       }
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
           
           
           std::cout << "Resizing DB from " << m_currDB->DBSize << " to " << expectedExtraLength+m_currDB->DBSize << std::endl;
           ResizeDB(expectedExtraLength+m_currDB->DBSize);
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
       for(uint i = 0; i < extraBuffSize;i++)
       {
           m_currDB->DBBuffer[allowedWriteOffset+tableLength+i] = tmpBuffer[i];
       }
       delete[] tmpBuffer;
       uint64_t newTableOffset = (allowedWriteOffset+tableLength);
       int size = 0;
       uint64_t newEntryOffset = 0;
       size_t* results = UnsignedString::Search(m_currDB->DBBuffer, allowedWriteOffset+tableLength, m_currDB->DBSize, entrySig, 6, size);
       
       std::cout << "Found " << size << " other tables" << std::endl;
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
           m_currDB->DBBuffer[ENTWrite+i] = entrySig[i];
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
           m_currDB->DBBuffer[ENTWrite+6+4+tableNameSize+8+i] = tableSig[i];
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
       delete[] entrySig;
       delete[] tableSig;
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
           uchar* tmp_buffer = new uchar[m_currDB->DBSize-EOT];
           uint64_t tmp_buffer_size = m_currDB->DBSize-EOT;
           for(uint64_t i = 0; i < m_currDB->DBSize-EOT;i++)
           {
               tmp_buffer[i] = m_currDB->DBBuffer[EOT+i];
           }
           
           m_RewriteEOT(tableName,newEOT);
           ResizeDB(expandSize+m_currDB->DBSize,1);
           m_DirectWrite(newEOT,tmp_buffer_size,tmp_buffer);
           delete[] tmp_buffer;
           m_NullFill(EOT,expandSize);
           m_UpdateEntryTableOffset(expandSize);
           m_FixEntryTable(expandSize);
           
           uchar* rowSig = new uchar[6]{
           'R',
           static_cast<uchar>('\0'),
           'O',
           static_cast<uchar>('\0'),
           'W',
           static_cast<uchar>('\0')
           };
           //uint64_t writeOffset = m_GetLastColumnEndOffset(tableName);
           uint64_t writeOffset = m_GetNewRowWriteOffset(tableName);
           m_DirectWrite(writeOffset, 6, rowSig);
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
                    if(row.keys.at(i).value.length() < maxData)
                    {
                        m_DirectWriteNullFill(newWriteOffset,row.keys.at(i).value.length(),maxData,UnsignedBuffer::UcharFromString(row.keys.at(i).value));
                    }
                    else
                    {
                        m_DirectWrite(newWriteOffset,maxData,UnsignedBuffer::UcharFromString(row.keys.at(i).value));
                        m_AddError("Data is too large for column - Writing untill maxSize is hit");
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
       size_t rowCount = m_GetRowCount(tableName);
       for(size_t i = 0; i < rowCount; i++)
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
   
   bool DeleteRow(std::string tableName, Key where)
   {
       DBRow* rows = m_GetAllRows(tableName);
       size_t rowCount = m_GetRowCount(tableName);
       for(size_t i = 0; i < rowCount; i++)
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
               for(size_t i = 0; i < length;i++)
               {
                   m_currDB->DBBuffer[offset+i] = m_currDB->DBBuffer[offset+length+i];
               }
               ResizeDB(m_currDB->DBSize-length,1);
               return 1;
               
           }
       }
       return false;
   }
   
   
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
         std::ofstream out = std::ofstream("./backUp.db",std::ios::binary);
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
        size_t offset = m_GetLastColumnEndOffset(tableName);
        size_t EOT = m_GetEOT(tableName);
        int resultSize = 0;
        size_t* results = UnsignedString::Search(m_currDB->DBBuffer,offset,EOT,g_stoopidDBRowSig,6,resultSize);
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
    uint64_t m_GetRowLength(size_t EntryWithSig)
    {
        uint64_t size = 0;
        for(int i = 0; i < 8;i++)
        {
           size = (size | m_currDB->DBBuffer[EntryWithSig+6+7-i]) << (i == 7 ? 0 : 8);
        }
        return size;
    }
    
    
    uint64_t m_GetNewRowWriteOffset(std::string tableName)
    {
        size_t rowsStart = m_GetLastColumnEndOffset(tableName);
        size_t rowsSize = m_GetEOT(tableName)-rowsStart;
        int rows = 0;
        size_t* offsets = UnsignedString::Search(m_currDB->DBBuffer,rowsStart,rowsSize,g_stoopidDBRowSig,6,rows);
        size_t returnOffset = m_GetLastColumnEndOffset(tableName);
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
    void m_FixEntryTable(size_t expandSize)
    {
        uint32_t offsetsSize = 0;
        uint64_t* offsets = m_GetAllEntryOffsets(offsetsSize);
        size_t namesSize = 0;
        std::string* names = m_ScanForTableNames(namesSize);
        if(offsetsSize < namesSize)
        {
            m_AddError("[Non-Fatal] Entry table is missing entries - Full rebuild of entry table is recommended");
        }
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
                      size_t size = 0;
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
        delete[] names;
        delete[] offsets;
        
    }
    
    /*
     *@param Scans for names of tables without using the entry table - Useful if you suspect the entry table to be corrupted - However it does depend on the size of the tables being recorded acurately
     *
     */
    std::string* m_ScanForTableNames(size_t &arraySize)
    {
       uint32_t count = m_GetTableCount();
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
    uint64_t* m_ScanForTableNameOffsets(size_t &arraySize)
    {
       uint32_t count = m_GetTableCount();
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
        for(size_t i = 0; i < rows; i++)
        {
            for(size_t u = 0; i < dbRows[i].keys.size();u++)
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
        size_t offset = m_GetColumnsStart(tableName);
        size_t columnCount = m_GetColumnCount(tableName);
        std::string* names = m_GetAllColumnNames(tableName);
        for(size_t i = 0; i < columnCount; i++)
        {
            UnsignedString tmp = m_ReadUnsignedString(offset,255);
            size_t columnSize = m_ReadUnsingedValue(4, offset+tmp.length());
            size_t nextOffset = offset+columnSize;
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
    
    DBRow* m_GetAllRows(std::string tableName)
    {
        size_t rowCount = m_GetRowCount(tableName);
        DBRow* rows = new DBRow[rowCount];
        size_t offset = m_GetLastColumnEndOffset(tableName);
        size_t EOT = m_GetEOT(tableName);
        int resultSize = 0;
        size_t* results = UnsignedString::Search(m_currDB->DBBuffer,offset,EOT,g_stoopidDBRowSig,6,resultSize);
        for(size_t i = 0; i < rowCount; i++)
        {
            size_t rowOffset = 0;
            size_t rowLength = m_GetRowLength(results[i]);
            while(true)
            {
                std::string columnName = m_GetColumnNameFromID(tableName,m_ReadUnsingedValue(4,results[i]+6+8+rowOffset));
                size_t columnMAX = m_GetColumnDataMaxSize(tableName,columnName);
                DataType type = DataType(m_GetColumnType(tableName,columnName));
                std::string dataString = m_ReadString(results[i]+6+8+4+rowOffset,columnMAX);
                switch(type)
                {
                    case DataType::INT:
                        dataString = std::to_string(static_cast<int64_t>(std::atoi(dataString.c_str())));
                    break;
                    case DataType::VARCHAR:
                        //Thats fine XD
                    break;
                    case DataType::TIME_DATE:
                        //TODO
                    break;
                    default:
                        std::string tmpS = "[Non-Fatal] Could Not Get DataType of Column: ";
                        tmpS+="<" + tableName + "," + columnName + ">";
                        m_AddError(tmpS);
                    break;
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
    
    std::string m_GetColumnNameFromID(std::string tableName, size_t id)
    {
        std::string name = "";
        std::string* names = m_GetAllColumnNames(tableName);
        name = names[id];
        delete[] names;
        return name;
        
    }
    
    uchar m_GetColumnSettings(std::string tableName, std::string columnName)
    {
       size_t offset =  m_GetColumnOffset(tableName,columnName);
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
    std::vector<T> m_VectorFromArray(T* arr, size_t len)
    {
        std::vector<T> tmpVec = std::vector<T>();
        for(size_t i = 0; i < len; i++)
        {
            tmpVec.push_back(arr[i]);
        }
        return tmpVec;
    }
    
    uchar m_GetColumnType(std::string tableName, std::string columnName)
    {
       return m_ReadUnsingedValue(1,m_GetColumnOffset(tableName,columnName)+columnName.length()+1+4);  
    }
    
    bool m_WriteAutoAndUpdateOffset(std::string tableName, size_t &offset)
    {
        size_t rowCount = m_GetRowCount(tableName);
        std::string primKey = m_GetPrimaryKey(tableName);
        if(primKey == "")
        {
          m_AddError("[Error Non-Fatal] No PrimkeyColumn Found");
          return false;  
        }
        
        size_t currAutoInc = 0;
        if(rowCount-1 > 0)
        {
            DBRow* rows = m_GetAllRows(tableName);
            if(primKey != "" && rows[rowCount].Find(primKey) != *nokey)
            {
                currAutoInc = std::stoi(rows[rowCount].Find(primKey).value);
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
        m_DirectWrite(offset,4,(uchar*)(&id));
        m_DirectWriteNullFill(offset+4,8,m_GetColumnDataMaxSize(tableName,primKey),(uchar*)(&currAutoInc));
        offset+=4+m_GetColumnDataMaxSize(tableName,primKey);
        return true;
        }
        else
        {
            m_AddError("[ERROR] Couldn't get ID of table: " + primKey);
            return false;
        }
    }
    
    size_t m_CalculateSpaceForUnassignedColumns(std::string tableName, std::vector<Key> assignedColumns)
    {
        
        size_t extraSize = 0;
        
        //For all columns marked as NOT NULL
        uint32_t columnCount = m_GetColumnCount(tableName);
        std::string* names = m_GetAllColumnNames(tableName);
        
        std::vector<std::string> tmpVec = m_VectorFromArray<std::string>(names, columnCount);
        
        for(size_t i = 0; i < assignedColumns.size(); i++)
        {
            std::string name = assignedColumns.at(i).name;
            for(size_t u = 0; u < tmpVec.size();u++)
            {
                if(tmpVec.at(u) == name)
                {
                    tmpVec.at(u) = "";
                }
            }
            
        }
        for(size_t i = 0; i < tmpVec.size();i++)
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
    
    size_t m_ReadUnsingedValue(uint bytesToRead, size_t offset)
    {
        size_t data = 0;
        for(uint i = 0; i < bytesToRead;i++)
        {
            data = (data | m_currDB->DBBuffer[offset+bytesToRead-1-i]) << (i == bytesToRead-1 ? 0: 8);
        }
        return data;
    }
    
    uint32_t m_GetRowCount(std::string tableName)
    {
        size_t offset = m_GetLastColumnEndOffset(tableName);
        size_t EOT = m_GetEOT(tableName);
        int resultSize = 0;
        size_t* results = UnsignedString::Search(m_currDB->DBBuffer,offset,EOT,UnsignedString(g_stoopidDBRowSig,6),resultSize);
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
        for(size_t i = 0; i < realLength;i++)
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
     */
    UnsignedString m_ReadUnsignedString(uint64_t _offset, uint maxRead)
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
        UnsignedString nullStr = UnsignedString(static_cast<uchar>('\0'));
        UnsignedString utmp = UnsignedString(tmp);
        utmp += nullStr;
        return utmp; 
    }
    
    void ResizeDB(size_t newSize, bool echo=false)
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
    
    size_t TableExist(std::string name)
    {
       uint64_t Offset = 0;
       uchar* sig = new uchar[8]{'\0',static_cast<uchar>('\xFF'),static_cast<uchar>('\xFF'),'\0','\0',static_cast<uchar>('\xFF'),static_cast<uchar>('\xFF'),'\0'};
       UnsignedString uname = UnsignedString(name);
       int resultSize = 0;
       
       size_t* results = UnsignedString::Search(m_currDB->DBBuffer, GetEntryPointer(), m_currDB->DBSize,sig,8,resultSize);
       if(resultSize == 0)
       {
           std::cout << TERMINAL_RED << "DB Corrupted!! - No EntryTableSignature found!!" << TERMINAL_NOCOLOR << std::endl;
           delete[] results;
           delete[] sig;
           return 0;
       }
       size_t end = results[0];
       
       //result = UnsignedString::Search(m_currDB->DBBuffer, GetEntryPointer(), end, UnsignedBuffer(name.c_str(),name.length()).GetBuffer(), name.length(), resultSize);
       size_t* result = UnsignedString::Search(m_currDB->DBBuffer, GetEntryPointer(), end, uname, resultSize); //Sigh - The buffer that holds the name deletes it self in the middle if a while loop... WTF!!!!!!! - Fixed for some reason the overloaded "=" operator was used instead of the copyconstructor
       
       if(resultSize == 0)
       {
           delete[] results;
           delete[] result;
           delete[] sig;
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
       delete[] sig;
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
