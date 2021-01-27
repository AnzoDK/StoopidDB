#include <iostream>
#include "stoopidDB.h"

int main()
{
    DBManager* dbMan = new DBManager();
    dbMan->CreateDB("./local_db.sdb");
    dbMan->LoadDB("./local_db.sdb");
    DBColumn* columns = new DBColumn[3];
    columns[0] = DBColumn("id",DataType::INT, 8, ColumnSettings::NOT_NULL | ColumnSettings::AUTO_INCREMENT | ColumnSettings::PRIMARY_KEY);
    columns[1] = DBColumn("Name", DataType::VARCHAR, 256, ColumnSettings::NOT_NULL);
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
    int resultSize = 0;
    DBRow* result = dbMan->GetAllRowsFromTable("Users",resultSize);
    for(size_t i = 0; i < resultSize;i++)
    {
         for(size_t u = 0; u < result[i].keys.size();u++)
         std::cout << "DB Returned key: " << result[i].keys.at(u).name << " : " << result[i].keys.at(u).value << std::endl;
    }
    delete[] result;
    delete dbMan;
}
