///////////////////////////////////
// Copyright 2012 Peter Atechian //
// Licensed under GPLv3          //
///////////////////////////////////

#include "wzmain.hpp"
#include "wzcrypto.hpp"
#include <iostream>

#include "..\Memory\MemoryPoolMan.hpp"

namespace WZ {
	int Version = 0;
path Path;
Node Base;
vector<path> Paths;
bool Lazy;
class File {
public:
    File(Node n);
	void Close();

private:
	void Init(Node n);
    void Directory(Node n);

    uint32_t fileStart;
    MapFile file;
};

/*bool CheckLoadedFile(const std::string& str)
{
	//auto findRet = str.find("Map2");
	std::vector<std::string> targetFind{ "Base", "Item" };
	for (const auto& t : targetFind)
	{
		int findRes = str.find(t);
		if (findRes >= 0 && findRes < str.size())
			return true;
	}
	return true;
}*/

void File::Close()
{
	file.CloseFile();
}

File::File(Node n) {
	Init(n);
}

void File::Init(Node n) {
    path filename = Path / path(n.Name() + ".wz");

	//std::cout << "[WzFile::Init]File Name : " << filename << std::endl;
	/*if (!CheckLoadedFile(filename.string()))
		return;*/
	if (!exists(filename)) return;/* die();*/
    file.Open(filename.string());
	if (file.Read<uint32_t>() != *(uint32_t *)"PKG1") throw;
    uint64_t fileSize = file.Read<uint64_t>();
    fileStart = file.Read<uint32_t>();
    file.Seek(fileStart);
    if (!Version) {
        EncVersion = file.Read<int16_t>();
        int32_t count = file.ReadCInt();
        uint32_t c = 0;
        for (int k = 0; k < count; ++k) {
            uint8_t type = file.Read<uint8_t>();
            if (type == 3) {
                file.ReadEncString();
                file.ReadCInt();
                file.ReadCInt();
                file.Read<uint32_t>();
                continue;
            } else if (type == 4) {
                file.ReadEncString();
                file.ReadCInt();
                file.ReadCInt();
#pragma warning(disable:4244)  
                c = file.Tell();
#pragma warning(default:4244)  
                break;
            } else
                die();
        }
        if (c == 0) die();
        bool success = false;
        for (uint8_t j = 0; j < 3 && !success; ++j) {
            Key = Keys[j];
            AKey = AKeys[j];
            WKey = WKeys[j];
            for (WZ::Version = 0; Version < 512; ++Version) {
                string s = to_string(Version);
                size_t l = s.length();
                VersionHash = 0;
                for (int i = 0; i < l; ++i) VersionHash = 32 * VersionHash + s[i] + 1;
                uint32_t result = 0xFF ^ (VersionHash >> 24) ^ (VersionHash << 8 >> 24)
                                  ^ (VersionHash << 16 >> 24) ^ (VersionHash << 24 >> 24);
                if (result == EncVersion) {
                    file.Seek(c);
                    uint32_t off = file.ReadOffset(fileStart);
                    if (off > fileSize) continue;
                    file.Seek(off);
                    uint8_t a = file.Read<uint8_t>();
                    if (a != 0x73) continue;
                    string ss = file.ReadEncString();
                    if (ss != "Property") continue;
                    success = true;
                    break;
                }
            }
        }
        if (!success) die();
        file.Seek(fileStart + 2);
	}
	else {
		int16_t eversion = file.Read<int16_t>();
		if (eversion != EncVersion) {
			printf("Wz Version Error, file name = %s\n", filename.string().c_str());
			die();
		}
    }
    if (file.ReadCInt() == 0) die();
    file.Seek(fileStart + 2);
    Directory(n);
    delete this;
}

void File::Directory(Node n) {
    int32_t count = file.ReadCInt();
    if (count == 0) {
        new File(n);
		//auto f = (File*)malloc(sizeof(File));
		//f->File(n);
		/*auto ch = gMemoryPoolMan->Allocate(sizeof(File));
		new(p) File(n);*/
        return;
    }
#pragma warning(disable:4244)
    vector<Node> dirs;
    n.Reserve(count);
    for (int i = 0; i < count; ++i) {
        char * name = nullptr;
        uint8_t type = file.Read<uint8_t>();
        if (type == 1) {
            file.Skip(10);
            die();
            continue;
        } else if (type == 2) {
            int32_t s = file.Read<int32_t>();
            uint32_t p = file.Tell();
            file.Seek(fileStart + s);
            type = file.Read<uint8_t>();
            name = file.ReadEncString();
            file.Seek(p);
        } else if (type == 3)
            name = file.ReadEncString();
        else if (type == 4)
            name = file.ReadEncString();
        else
            die();
        uint32_t fsize = file.ReadCInt();
        uint32_t checksum = file.ReadCInt();
        uint32_t off = file.ReadOffset(fileStart);
        if (type == 3) { dirs.emplace_back(n.g(name, i)); } else if (type == 4) {
            size_t len = strlen(name);
            name[len - 4] = '\0';
            new Img(file, n.g(name, i), fsize, off);
        } else
            die();
    }
    for (Node nn : dirs) { Directory(nn); }
#pragma warning(default:4244)
}

void Load(string name) {
    Base.InitTop(name);
    new File(Base);
    if (!Lazy) {
        for (Img * img : Img::Imgs)img->Parse();
        Base.Resolve();
    }
}

void Init(bool lazy) {
    GenKeys();
    Lazy = lazy;
    for (path p : Paths) {
        Path = p;
        /*if (exists(Path / path("Data.wz"))) {
            Load("Data");
            return;
        }*/
        if (exists(Path / path("Base.wz"))) {
            Load("Base");
            return;
        }
    }
    die();
}

void AddPath(const string &path) { Paths.push_back(path); }
}