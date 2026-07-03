#ifndef HFS_H
#define HFS_H

typedef uint32_t HFSCatalogNodeID;
typedef uint32_t        FourCharCode;
typedef FourCharCode  OSType;

enum {
    kHFSPlusFolderRecord        = 0x0001,
    kHFSPlusFileRecord          = 0x0002,
    kHFSPlusFolderThreadRecord  = 0x0003,
    kHFSPlusFileThreadRecord    = 0x0004
};


struct Point {
  int16_t              v;
  int16_t              h;
} __attribute__((__packed__));
typedef struct Point  Point;
 
struct ExtendedFileInfo {
  int16_t    reserved1[4];
  uint16_t    extendedFinderFlags;
  int16_t    reserved2;
  int32_t    putAwayFolderID;
} __attribute__((__packed__));
typedef struct ExtendedFileInfo   ExtendedFileInfo;



struct FileInfo {
  OSType    fileType;           /* The type of the file */
  OSType    fileCreator;        /* The file's creator */
  uint16_t    finderFlags;
  Point     location;           /* File's location in the folder. */
  uint16_t    reservedField;
} __attribute__((__packed__));
typedef struct FileInfo   FileInfo;

struct Rect {
  int16_t              top;
  int16_t              left;
  int16_t              bottom;
  int16_t              right;
} __attribute__((__packed__));
typedef struct Rect   Rect;

struct HFSUniStr255 {
    uint16_t  length;
    uint16_t unicode[255];
} __attribute__((__packed__));
typedef struct HFSUniStr255 HFSUniStr255;
typedef const  HFSUniStr255 *ConstHFSUniStr255Param;

struct FolderInfo {
  Rect      windowBounds;       /* The position and dimension of the */
                                /* folder's window */
  uint16_t    finderFlags;
  Point     location;           /* Folder's location in the parent */
                                /* folder. If set to {0, 0}, the Finder */
                                /* will place the item automatically */
  uint16_t    reservedField;
} __attribute__((__packed__));
typedef struct FolderInfo   FolderInfo;

struct ExtendedFolderInfo {
  Point     scrollPosition;     /* Scroll position (for icon views) */
  int32_t    reserved1;
  uint16_t    extendedFinderFlags;
  int16_t    reserved2;
  int32_t    putAwayFolderID;
} __attribute__((__packed__));
typedef struct ExtendedFolderInfo   ExtendedFolderInfo;

struct HFSPlusBSDInfo {
    uint32_t  ownerID;
    uint32_t  groupID;
    uint8_t   adminFlags;
    uint8_t   ownerFlags;
    uint16_t  fileMode;
    union {
        uint32_t  iNodeNum;
        uint32_t  linkCount;
        uint32_t  rawDevice;
    } special;
} __attribute__((__packed__));
typedef struct HFSPlusBSDInfo HFSPlusBSDInfo;

struct HFSPlusExtentDescriptor {
    uint32_t startBlock;
    uint32_t blockCount;
} __attribute__((__packed__));
typedef struct HFSPlusExtentDescriptor HFSPlusExtentDescriptor;

typedef HFSPlusExtentDescriptor HFSPlusExtentRecord[8];

struct HFSPlusForkData {
  uint64_t logicalSize;
  uint32_t clumpSize;
  uint32_t totalBlocks;
  HFSPlusExtentRecord extents;
} __attribute__((__packed__));
typedef struct HFSPlusForkData HFSPlusForkData;
 
struct HFSPlusVolumeHeader {
  uint16_t signature;
  uint16_t version;
  uint32_t attributes;
  uint32_t lastMountedVersion;
  uint32_t journalInfoBlock;
 
  uint32_t createDate;
  uint32_t modifyDate;
  uint32_t backupDate;
  uint32_t checkedDate;
 
  uint32_t fileCount;
  uint32_t folderCount;
 
  uint32_t blockSize;
  uint32_t totalBlocks;
  uint32_t freeBlocks;
 
  uint32_t nextAllocation;
  uint32_t rsrcClumpSize;
  uint32_t dataClumpSize;
  HFSCatalogNodeID nextCatalogID;
 
  uint32_t writeCount;
  uint64_t encodingsBitmap;
 
  uint32_t finderInfo[8];
 
  HFSPlusForkData allocationFile;
  HFSPlusForkData extentsFile;
  HFSPlusForkData catalogFile;
  HFSPlusForkData attributesFile;
  HFSPlusForkData startupFile;
} __attribute__((__packed__));
typedef struct HFSPlusVolumeHeader HFSPlusVolumeHeader;

enum {
    kBTLeafNode       = -1,
    kBTIndexNode      =  0,
    kBTHeaderNode     =  1,
    kBTMapNode        =  2
};

struct BTNodeDescriptor {
    uint32_t    fLink;
    uint32_t    bLink;
    int8_t     kind;
    uint8_t     height;
    uint16_t    numRecords;
    uint16_t    reserved;
} __attribute__((__packed__));
typedef struct BTNodeDescriptor BTNodeDescriptor;

#define kHFSCaseFolding 0xCF
#define kHFSBinaryCompare 0xBC

struct BTHeaderRec {
    uint16_t    treeDepth;
    uint32_t    rootNode;
    uint32_t    leafRecords;
    uint32_t    firstLeafNode;
    uint32_t    lastLeafNode;
    uint16_t    nodeSize;
    uint16_t    maxKeyLength;
    uint32_t    totalNodes;
    uint32_t    freeNodes;
    uint16_t    reserved1;
    uint32_t    clumpSize;      // misaligned
    uint8_t     btreeType;
    uint8_t     keyCompareType;
    uint32_t    attributes;     // long aligned again
    uint32_t    reserved3[16];
} __attribute__((__packed__));
typedef struct BTHeaderRec BTHeaderRec;

struct HFSPlusCatalogFolder {
    int16_t              recordType;
    uint16_t              flags;
    uint32_t              valence;
    HFSCatalogNodeID    folderID;
    uint32_t              createDate;
    uint32_t              contentModDate;
    uint32_t              attributeModDate;
    uint32_t              accessDate;
    uint32_t              backupDate;
    HFSPlusBSDInfo      permissions;
    FolderInfo          userInfo;
    ExtendedFolderInfo  finderInfo;
    uint32_t              textEncoding;
    uint32_t              folderCount;
} __attribute__((__packed__));
typedef struct HFSPlusCatalogFolder HFSPlusCatalogFolder;

struct HFSPlusCatalogFile {
    int16_t              recordType;
    uint16_t              flags;
    uint32_t              reserved1;
    HFSCatalogNodeID    fileID;
    uint32_t              createDate;
    uint32_t              contentModDate;
    uint32_t              attributeModDate;
    uint32_t              accessDate;
    uint32_t              backupDate;
    HFSPlusBSDInfo      permissions;
    FileInfo            userInfo;
    ExtendedFileInfo    finderInfo;
    uint32_t              textEncoding;
    uint32_t              reserved2;
 
    HFSPlusForkData     dataFork;
    HFSPlusForkData     resourceFork;
} __attribute__((__packed__));
typedef struct HFSPlusCatalogFile HFSPlusCatalogFile;

struct HFSPlusCatalogThread {
    int16_t              recordType;
    int16_t              reserved;
    HFSCatalogNodeID    parentID;
    HFSUniStr255        nodeName;
} __attribute__((__packed__));
typedef struct HFSPlusCatalogThread HFSPlusCatalogThread;

struct HFSPlusExtentKey {
    uint16_t              keyLength;
    uint8_t               forkType;
    uint8_t               pad;
    HFSCatalogNodeID    fileID;
    uint32_t              startBlock;
} __attribute__((__packed__));
typedef struct HFSPlusExtentKey HFSPlusExtentKey;

struct HFSPlusCatalogKey {
    uint16_t              keyLength;
    HFSCatalogNodeID    parentID;
    HFSUniStr255        nodeName;
} __attribute__((__packed__));
typedef struct HFSPlusCatalogKey HFSPlusCatalogKey;

#endif