///////////////////////////////////
// Copyright 2012 Peter Atechian //
// Licensed under GPLv3          //
///////////////////////////////////
#pragma once
#include <string>
#include <cstdint>
#include <mutex>
#include "sprite.hpp"

namespace WZ {
class Node {
public:
    class Data;
    // Public functions
    Node();
    Node(const Node &);
    Node operator=(const Node &);
    Node operator[](std::string) const;
    Node operator[](const char *) const;
    Node operator[](char *) const;
    Node operator[](int) const;

	void ReleaseData();

    std::string Name() const;
    operator bool() const;
    operator std::string() const;
    operator double() const;
    operator int() const;
    // operator Sprite() const;
    // operator Sound() const;
    Node begin() const;
    Node end() const;
    Node & operator++();
    Node & operator--();
    bool operator==(const Node &) const;
    bool operator!=(const Node &) const;
    Node & operator*();

    // For internal use only
    Node(Data *);
    Node(Data &);
    void InitTop(std::string);
    void Assign(const Node &);
    void Set(char *);
    void SetUOL(char *);
    void Set(double);
    void Set(int);
    void Set(class Img *);
    // void Set(const class Sound&);
    // void Set(const class Sprite&);
    void Resolve();
    void Reserve(int);
    Node g(char *, int n);
	void* pFilePtr;
private:
    Data * data;
	std::recursive_mutex m_lock;
	std::mutex m_lock2;
};
//extern Node Base;
//extern uint16_t Version;
//void Init(bool lazy = true);
//void AddPath(std::string);
}
