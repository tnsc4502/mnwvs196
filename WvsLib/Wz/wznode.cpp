///////////////////////////////////
// Copyright 2012 Peter Atechian //
// Licensed under GPLv3          //
///////////////////////////////////

#include "wzmain.hpp"
#include "..\Memory\MemoryPoolMan.hpp"
namespace WZ {
class Node::Data {
public:
    union vt {
        double dreal;
        int ireal;
        char * string;
        Img * img;
    } value;
    Data * parent = nullptr;
    char * name;
    Data * children = nullptr;
    uint16_t num;
    enum class Type : uint8_t { none, dreal, ireal, string, uol, sprite, sound, img } type;

private:
    Data(const Node::Data &);

public:

	void Release()
	{
		//printf("On Release");
		if (type == Type::string) {
			WzMemoryPoolMan::GetInstance()->DestructArray(value.string);
			value.string = nullptr;
		}
			//delete[] value.string;*/
		if (type == Type::img) {
			WzMemoryPoolMan::GetInstance()->DestructArray((char*)value.img);
		}
			/*delete this->value.img;*/
		for(int i = 0; i < num; ++i)
			children[i].Release();
		if(children)
			WzMemoryPoolMan::GetInstance()->DestructArray(children);
		WzMemoryPoolMan::GetInstance()->DestructArray(name);
		children = nullptr;
		num = 0;
		//this->~Data();
	}

	~Data()
	{
	    /*if (type == Type::string)
			delete value.img;*/
		/*if (children)
			delete children;*/
	}
};

Node::Node() : data(nullptr) {}

Node::Node(Data * data) : data(data) {}

Node::Node(Data & data) : data(&data) {}

Node::Node(const Node & other) : data(other.data) {}

Node Node::operator=(const Node & other) { return data = other.data; }

//std::mutex m_mtxNodeLock;

Node Node::operator[](const char * key) const {
	//std::lock_guard<std::mutex> lock(m_mtxNodeLock);
    if (!data) return Node();
    if (strcmp(key, "..") == 0) return data->parent;
    if (strcmp(key, ".") == 0) return *this;
    if (data->type == Data::Type::img) {
        data->type = Data::Type::none;
        data->value.img->Parse();
    }
    if (!data->children) return Node();
    for (int i = 0; i < data->num; ++i) {
        if (data->children[i].name && strcmp(data->children[i].name, key) == 0) return data->children[i];
    }
    return Node();
}

Node Node::operator[](string key) const { return operator[](key.c_str()); }
Node Node::operator[](char * key) const { return operator[]((const char *)key); }

Node Node::operator[](int key) const { return operator[](to_string(key).c_str()); }



void Node::ReleaseData()
{
	//std::cout << "Addr = " << &(::gMemoryPoolMan) << std::endl;

	data->Release();
	if(data->type == Data::Type::none && data->value.img)
		data->type = Data::Type::img;
}

Node Node::g(char * key, int n) {
    data->children[n].name = key;
    data->children[n].parent = this->data;
    return data->children[n];
}

string Node::Name() const {
    if (!data || !data->name) return string();
    return data->name;
}

void Node::InitTop(string s) {
#pragma warning(disable:4267)
    data = (Data *)calloc(1, sizeof(Data));
    data->name = AllocString(s.length() + 1);
    strcpy(data->name, s.data());
    data->parent = this->data;
#pragma warning(default:4267)
}

void Node::Assign(const Node & other) { data = other.data; }

Node::operator bool() const { return data == nullptr; }

Node::operator string() const {
    if (!data) return string();
    switch (data->type) {
    case Data::Type::ireal: return to_string(data->value.ireal);
    case Data::Type::dreal: return to_string(data->value.dreal);
    case Data::Type::string: return data->value.string;
    default: return string();
    }
}

Node::operator double() const {
    if (!data) return 0;
    if (data->type == Data::Type::ireal)
        return data->value.ireal;
    else if (data->type == Data::Type::dreal)
        return data->value.dreal;
    else
        return 0;
}

Node::operator int() const {
    if (!data) return 0;
    if (data->type == Data::Type::ireal)
        return data->value.ireal;
    else if (data->type == Data::Type::dreal)
        return (int)data->value.dreal;
    else
        return 0;
}

/*Node::operator Sprite() const {
if (!data) return Sprite();
return data->sprite;
}

Node::operator Sound() const {
if (!data) return Sound();
return data->sound;
}*/

void Node::Set(char * v) {
    if (!v) v = AllocString(1);
    data->value.string = v;
    data->type = Data::Type::string;
}

void Node::SetUOL(char * v) {
    data->value.string = v;
    data->type = Data::Type::uol;
}

void Node::Set(double v) {
    data->value.dreal = v;
    data->type = Data::Type::dreal;
}

void Node::Set(int v) {
    data->value.ireal = v;
    data->type = Data::Type::ireal;
}

/*void Node::Set(const Sprite& v) {
data->sprite = v;
}

void Node::Set(const Sound& v) {
data->sound = v;
}*/

void Node::Set(Img * img) {
    data->value.img = img;
    data->type = Data::Type::img;
}

void Node::Resolve() {
	std::lock_guard<std::recursive_mutex>  lock(this->m_lock);
    if (!data) return;
    if (data->type == Data::Type::uol) {
        char * s = data->value.string;
		if (!s) return;
        static char * parts[10];
        int n = 1;
        char * it = s;
        parts[0] = s;
        while (*it != '\0') {
            if (*it == '/') {
                *it = '\0';
                parts[n] = it + 1;
                ++n;
            }
            ++it;
        }
        Node nn = data->parent;
        for (int i = 0; i < n; ++i) {
            if (!nn) break;
            nn = nn[parts[i]];
        }
        if (nn) data = nn.data;
    } else {
        for (Node n : *this) { n.Resolve(); }
    }
}

void Node::Reserve(int n) {
    /*static Data * d = nullptr;
    static size_t remain = 0;
    if (remain < n) {
        d = (Data *)calloc(0x10000, sizeof(Data));
        remain = 0x10000;
    }
    data->children = d;
    remain -= n;
    d += n;*/
    data->num = n;
	data->children = (Data*)WzMemoryPoolMan::GetInstance()->AllocateArray(n * sizeof(Data));
}

Node Node::begin() const {
    if (!data) return nullptr;
    if (data->type == Data::Type::img) {
        data->type = Data::Type::none;
        data->value.img->Parse();
    }
    return data->children;
}

Node Node::end() const {
    if (!data) return nullptr;
    if (data->type == Data::Type::img) {
        data->type = Data::Type::none;
        data->value.img->Parse();
    }
    return data->children + data->num;
}

Node & Node::operator++() {
    ++data;
    return *this;
}

Node & Node::operator--() {
    --data;
    return *this;
}

bool Node::operator==(const Node & other) const { return data == other.data; }

bool Node::operator!=(const Node & other) const { return data != other.data; }

Node & Node::operator*() { return *this; }
}