## 数据库表设计
# User表
字段名称	字段类型	                字段说明	         约束
id	        INT	                       用户id	        PRIMARY KEY、AUTO_INCREMENT
name	    VARCHAR(50)	               用户名	        NOT NULL, UNIQUE
password	VARCHAR(50)	               用户密码	        NOT NULL
state	E   NUM('online', 'offline')   当前登录状态	     DEFAULT 'offline'

CREATE TABLE User (
    id INT PRIMARY KEY AUTO_INCREMENT,
    name VARCHAR(50) NOT NULL UNIQUE,
    password VARCHAR(50) NOT NULL,
    state ENUM('online', 'offline') DEFAULT 'offline'
);

# Friend表

字段名称	字段类型	字段说明	        约束
userid	    INT	        用户id	    NOT NULL、联合主键
friendid	INT	        好友id	    NOT NULL、联合主键

CREATE TABLE Friend(
    userid INT NOT NULL,
    friendid INT NOT NULL,
    PRIMARY KEY (userid,friendid)
    );

# AllGroup

字段名称	字段类型	    字段说明	        约束
id	        INT	            组id	        PRIMARY KEY、AUTO_INCREMENT
groupname	VARCHAR(50)	    组名称	        NOT NULL, UNIQUE
groupdesc	VARCHAR(200)	组功能描述	    DEFAULT ''


CREATE TABLE AllGroup (
    id INT PRIMARY KEY AUTO_INCREMENT,
    groupname VARCHAR(50) NOT NULL UNIQUE,
    groupdesc VARCHAR(200) DEFAULT ''
);


# GroupUser表

字段名称	    字段类型	                字段说明	        约束
groupid	        INT	                        组id	        NOT NULL、联合主键
userid	        INT	                        组员id	        NOT NULL、联合主键
grouprole	    ENUM('creator', 'normal')	组内角色	    DEFAULT 'normal'


CREATE TABLE GroupUser (
    groupid INT NOT NULL,
    userid INT NOT NULL,
    grouprole ENUM('creator', 'normal') DEFAULT 'normal',
    PRIMARY KEY (groupid, userid)
);


# OfflineMessage表

字段名称	    字段类型	    字段说明	                    约束
userid	        INT	            用户id	                    NOT NULL
message	        VARCHAR(50)	    离线消息（存储Json字符串）	  NOT NULL

CREATE TABLE OfflineMessage (
    userid INT NOT NULL,
    message VARCHAR(50) NOT NULL,
    PRIMARY KEY (userid)
);

