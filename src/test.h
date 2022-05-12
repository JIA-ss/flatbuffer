#pragma once
#include "Monster_generated.h"
#include "flatbuffers/flatbuffer_builder.h"

#include <fstream>
#include <ios>
#include <istream>
#include <iterator>
#include <string>
#include <vcruntime_string.h>

using namespace MyGame::Sample;




void readFile(const char* path, std::string& content);
bool writeFile(const char* path, const std::string& content);

void readFlatBuffer()
{
    std::string content;
    readFile(SAVED_FILE_PATH, content);
    uint8_t *buffer_pointer  = (uint8_t*)content.data();
    
    auto monster = GetMonster(buffer_pointer);
    auto hp = monster->hp();
    auto mana = monster->mana();
    auto name = monster->name()->c_str();
    auto pos = monster->pos();
    auto x = pos->x();
    auto y = pos->y();
    auto z = pos->z();
}

void writeFlatBuffer()
{
    flatbuffers::FlatBufferBuilder builder(1024);

    auto weapon_one_name = builder.CreateString("Sword");
    short weapon_one_damage = 3;
    
    auto weapon_two_name = builder.CreateString("Axe");
    short weapon_two_damage = 5;
    
    // Use the `CreateWeapon` shortcut to create Weapons with all the fields set.
    auto sword = CreateWeapon(builder, weapon_one_name, weapon_one_damage);
    auto axe = CreateWeapon(builder, weapon_two_name, weapon_two_damage);

    // Serialize a name for our monster, called "Orc".
    auto name = builder.CreateString("Orc");
    
    // Create a `vector` representing the inventory of the Orc. Each number
    // could correspond to an item that can be claimed after he is slain.
    unsigned char treasure[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    auto inventory = builder.CreateVector(treasure, 10);


    // Place the weapons into a `std::vector`, then convert that into a FlatBuffer `vector`.
    std::vector<flatbuffers::Offset<Weapon>> weapons_vector;
    weapons_vector.push_back(sword);
    weapons_vector.push_back(axe);
    auto weapons = builder.CreateVector(weapons_vector);

    Vec3 points[] = { Vec3(1.0f, 2.0f, 3.0f), Vec3(4.0f, 5.0f, 6.0f) };
    auto path = builder.CreateVectorOfStructs(points, 2);

    // Create the position struct
    auto position = Vec3(1.0f, 2.0f, 3.0f);
    
    // Set his hit points to 300 and his mana to 150.
    int hp = 100;
    int mana = 20;

    // Finally, create the monster using the `CreateMonster` helper function
    // to set all fields.
    auto orc = CreateMonster(builder, &position, mana, hp, name, inventory,
                            Color_Red, weapons, Equipment_Weapon, axe.Union(),
                            path);
    
    builder.Finish(orc); // You could also call `FinishMonsterBuffer(builder, orc);`


        // This must be called after `Finish()`.
    uint8_t *buf = builder.GetBufferPointer();
    int size = builder.GetSize(); // Returns the size of the buffer that
                                // `GetBufferPointer()` points to.
    std::string content;
    content.resize(size);
    memcpy(content.data(), buf, size);
    writeFile(SAVED_FILE_PATH, content);
}






void readFile(const char* path, std::string& content)
{
    std::ifstream ifs(path, std::ios::in | std::ios::binary);
    if (ifs.is_open())
    {
        content.clear();
        content.assign(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
    }
    else
    {
        content = "";
    }
}

bool writeFile(const char* path, const std::string& content)
{
    std::ofstream ofs;
    ofs.open(path, std::ios::out | std::ios::trunc | std::ios::binary);

    if (ofs.is_open())
    {
        ofs << content;
        ofs.close();
        return true;
    }
    return false;
}