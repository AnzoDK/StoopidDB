#include <iostream>
#include "stoopidDB.h"

void RowPrinter(DBRow* row, int arraySize)
{
    for(size_t i = 0; i < arraySize;i++)
    {
         for(size_t u = 0; u < row[i].keys.size();u++)
         std::cout << "DB Returned key: " << row[i].keys.at(u).name << " : " << row[i].keys.at(u).value << std::endl;
    }
}

int main()
{
    DBManager* dbMan = new DBManager();
    dbMan->CreateDB("./local_db.sdb");
    dbMan->LoadDB("./local_db.sdb");
    DBColumn* columns = new DBColumn[3];
    columns[0] = DBColumn("id",DataType::INT, 8, ColumnSettings::NOT_NULL | ColumnSettings::AUTO_INCREMENT | ColumnSettings::PRIMARY_KEY);
    columns[1] = DBColumn("Name", DataType::VARCHAR, 50, ColumnSettings::NOT_NULL);
    columns[2] = DBColumn("Age", DataType::INT, 8, ColumnSettings::NOT_NULL);
    if(dbMan->CreateTable("Users",3,columns))
    {
        delete[] columns;
        DBRow row = DBRow();
        row.InsertData("Name","John Doe");
        row.InsertData("Age","35");
        if(!dbMan->InsertRow("Users",row))
        {
          std::cout << dbMan->Error() << std::endl;
        }
    }
    columns = new DBColumn[3];
    columns[0] = DBColumn("id",DataType::INT, 8, ColumnSettings::NOT_NULL | ColumnSettings::AUTO_INCREMENT | ColumnSettings::PRIMARY_KEY);
    columns[1] = DBColumn("Title", DataType::VARCHAR, 100, ColumnSettings::NOT_NULL);
    columns[2] = DBColumn("Category", DataType::VARCHAR, 8, ColumnSettings::NOT_NULL);
    if(dbMan->CreateTable("Posts",3,columns))
    {
        delete[] columns;
        DBRow row = DBRow();
        row.InsertData("Title","A title is all you need");
        row.InsertData("Category","TextPosts");
        if(!dbMan->InsertRow("Posts",row))
        {
          std::cout << dbMan->Error() << std::endl;
        }
        row = DBRow();
        row.InsertData("Title","The more titles the better");
        row.InsertData("Category","TextPosts");
        if(!dbMan->InsertRow("Posts",row))
        {
          std::cout << dbMan->Error() << std::endl;
        }
    }
    int resultSize = 0;
    DBRow* result = dbMan->GetAllRowsFromTable("Users",resultSize);
    RowPrinter(result,resultSize);
    delete[] result;
    result = dbMan->GetAllRowsFromTable("Posts",resultSize);
    RowPrinter(result,resultSize);
    delete[] result;
    delete dbMan;
}
