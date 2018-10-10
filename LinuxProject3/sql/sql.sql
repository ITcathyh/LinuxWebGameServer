CREATE DATABASE mygame;
USE mygame;

CREATE TABLE user (
  userid   BIGINT  AUTO_INCREMENT
  COMMENT '动态的id',
  username VARCHAR(12) NOT NULL
  COMMENT '账号',
  password VARCHAR(32) NOT NULL
  COMMENT '密码',
  level    SMALLINT DEFAULT 1
  COMMENT '等级',
  points   INT     DEFAULT 0
  COMMENT '经验',
  agility  SMALLINT DEFAULT 10
  COMMENT '敏捷值',
  strength SMALLINT DEFAULT 10
  COMMENT '力量值',
  HP INT DEFAULT 100
  COMMENT '血量',
  MP INT DEFAULT 10
  COMMENT '法力值',
  PRIMARY KEY (userid),
  INDEX (username)
)
  ENGINE = InnoDB
  DEFAULT CHARSET = utf8;

CREATE TABLE equipment (
  equipid BIGINT AUTO_INCREMENT
  COMMENT '动态的id',
  name        VARCHAR(18)  NOT NULL
  COMMENT '装备名',
  detail      VARCHAR(128) NOT NULL
  COMMENT '装备介绍',
  agility  SMALLINT DEFAULT 0
  COMMENT '敏捷值',
  strength SMALLINT DEFAULT 0
  COMMENT '力量值',
  HP INT DEFAULT 0
  COMMENT '血量',
  MP INT DEFAULT 0
  COMMENT '法力值',
  type        TINYINT      NOT NULL
  COMMENT '类型',
  PRIMARY KEY (equipid)
)
  ENGINE = InnoDB
  DEFAULT CHARSET = utf8;

CREATE TABLE monster (
  monsterid   BIGINT  AUTO_INCREMENT
  COMMENT '动态的id',
  monstername VARCHAR(18) NOT NULL
  COMMENT '怪兽名',
  agility     SMALLINT DEFAULT 2
  COMMENT '敏捷值',
  strength    SMALLINT DEFAULT 2
  COMMENT '力量值',
  level SMALLINT DEFAULT 1
  COMMENT '等级',
  HP INT DEFAULT 150
  COMMENT '血量',
  PRIMARY KEY (monsterid)
)
  ENGINE = InnoDB
  DEFAULT CHARSET = utf8;

CREATE TABLE map (
  mapid   BIGINT AUTO_INCREMENT
  COMMENT '动态的id',
  mapname VARCHAR(18)  NOT NULL
  COMMENT '地图名',
  detail  VARCHAR(256) NOT NULL
  COMMENT '地图介绍',
  PRIMARY KEY (mapid)
)
  ENGINE = InnoDB
  DEFAULT CHARSET = utf8;

CREATE TABLE map_monster (
  mapid     BIGINT NOT NULL
  COMMENT '地图id',
  monsterid BIGINT NOT NULL
  COMMENT '怪兽id',
  INDEX (mapid),
  INDEX (monsterid),
  KEY (mapid, monsterid)
)
  ENGINE = InnoDB
  DEFAULT CHARSET = utf8;

CREATE TABLE user_equip (
  userid  BIGINT NOT NULL
  COMMENT '用户id',
  equipid BIGINT NOT NULL
  COMMENT '装备id',
  wear    TINYINT(1) DEFAULT 0
  COMMENT '是否穿戴',
  INDEX (userid),
  INDEX (equipid)
)
  ENGINE = InnoDB
  DEFAULT CHARSET = utf8;

INSERT INTO user(username, password) VALUES ("1", "1");
INSERT INTO user(username, password) VALUES ("2", "2");
INSERT INTO equipment(name, detail, agility,HP, type) VALUES ("wowo", "test", 8,25,1);
INSERT INTO equipment(name, detail, agility,HP, type) VALUES ("aha", "gg", 6,30,1);
INSERT INTO equipment(name, detail, agility,HP, type) VALUES ("wuliao", "gg", 1,1,2);
INSERT INTO equipment(name, detail, agility,strength, type) VALUES ("new", "11", 10,25,3);
INSERT INTO equipment(name, detail, agility,strength, type) VALUES ("e1", "11", 8,7,3);
INSERT INTO equipment(name, detail, agility,strength, type) VALUES ("e2", "e2", 6,5,4);
INSERT INTO equipment(name, detail, agility,strength, type) VALUES ("e3", "e3", 8,8,5);
INSERT INTO monster(monstername, agility, strength) VALUES ("YC", 1 , 1);
INSERT INTO monster(monstername, agility, strength) VALUES ("gg", 10 , 15);
INSERT INTO monster(monstername, agility, strength) VALUES ("wawa", 8 , 8);
INSERT INTO monster(monstername, agility, strength, HP) VALUES ("YCBOSS", 30 , 30, 250);
INSERT INTO map(mapname, detail) VALUES ("neu", "neu");
INSERT INTO map(mapname, detail) VALUES ("459", "2");
INSERT INTO map_monster(mapid, monsterid) VALUES (1,1);
INSERT INTO map_monster(mapid, monsterid) VALUES (1,2);
INSERT INTO map_monster(mapid, monsterid) VALUES (1,3);
INSERT INTO map_monster(mapid, monsterid) VALUES (2,4);
INSERT INTO user_equip(userid, equipid) VALUES (1,1);
INSERT INTO user_equip(userid, equipid) VALUES (1,4);
INSERT INTO user_equip(userid, equipid, wear) VALUES (1,5,1);
INSERT INTO user_equip(userid, equipid, wear) VALUES (2,2,1);