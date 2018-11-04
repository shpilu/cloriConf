// 
// cloriConf MACROs definition 
// version: 1.0 
// Copyright (C) 2018 James Wei (weijianlhp@163.com). All rights reserved
//

#ifndef CLORIS_MACRO_H_
#define CLORIS_MACRO_H_

#define EVENT_INIT          0x00000001
#define EVENT_ADD           0x00000002
#define EVENT_DELETE        0x00000004
#define EVENT_UPDATE        0x00000008
#define EVENT_CHILDREN      0x00000010
#define EVENT_SELF_CHANGED  (EVENT_ADD | EVENT_DELETE | EVENT_UPDATE)
#define EVENT_CHANGED       (EVENT_ADD | EVENT_DELETE | EVENT_UPDATE | EVENT_CHILDREN)
#define EVENT_ALL           (EVENT_INIT | EVENT_ADD | EVENT_DELETE | EVENT_UPDATE | EVENT_CHILDREN)

// |...| FMT(4bit) | COMMENT(4bit) | SRC(4bit) |
#define SRC_LOCAL       0x00000001
#define SRC_DIRECT      0x00000002
#define SRC_ZK          0x00000004

#define FMT_JOML        0x00000010
#define FMT_JSON        0x00000020

// comment characters for joml/ini 
#define CMT_SHARP       0x00000100  // #
#define CMT_SLASH       0x00000200  // //
#define CMT_SEMICOLON   0x00000400  // ;
#define CMT_PERCENT     0x00000800  // %

#define SRC_MASK        0x0000000f
#define FMT_MASK        0x000000f0
#define CMT_MASK        0x00000f00

#endif // CLORIS_MACRO_H_ 

