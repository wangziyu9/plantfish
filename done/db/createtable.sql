CREATE TABLE Monitor_Point
(
Monitor_Point_ID int NOT NULL,
Monitor_Point_Name varchar(255) NOT NULL,
Monitor_Point_Location varchar(255),
Monitor_Point_Info varchar(255),
Monitor_Point_Remark varchar(255),
PRIMARY KEY (Monitor_Point_ID)
)


INSERT INTO Monitor_Point
(
Monitor_Point_ID,
Monitor_Point_Name,
Monitor_Point_Location,
Monitor_Point_Info,
Monitor_Point_Remark
)
VALUES
(
    001,
    'zzuli_library',
    '114,115',
    'library',
    ''
)


db.createUser(
{
user: 'admin',
pwd: 'Qwe456123',
roles: [{role: 'root', db: 'admin'}]
}
)