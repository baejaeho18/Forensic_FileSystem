#include <iostream>
#include <fstream>
#include <vector>

#include "byte_buffer.hpp"

using namespace std;
using namespace sys::io;

class Superblock
{
public:
	uint16_t bytes_per_sector;
	uint8_t sector_per_cluster;
	uint16_t resv_sector;		
	uint8_t no_fats;
	uint32_t sector_per_fat;
	uint32_t root_cluster_no;

	uint32_t cluster_size;		// each cluster
	uint64_t fat_size;			// each fat
	uint32_t fat_start_addr;	// start fat region
	uint64_t fat_end_addr;		// start data region

	Superblock(uint8_t* buffer, int size)
	{
		sys::io::byte_buffer bb((uint8_t*)buffer, 0, size);
		bb.skip(0x0b);
		bytes_per_sector = bb.get_uint16_le();
		sector_per_cluster = bb.get_uint8();
		resv_sector = bb.get_uint16_le();
		no_fats = bb.get_uint8();
		bb.skip(0x13);
		sector_per_fat = bb.get_uint32_le();
		bb.skip(0x04);
		root_cluster_no = bb.get_uint32_le();
		
		cluster_size = bytes_per_sector * sector_per_cluster;
		fat_size = bytes_per_sector * sector_per_fat;
		fat_start_addr = bytes_per_sector * resv_sector;
		fat_end_addr = fat_start_addr + fat_size * no_fats;
	}
};

class Cluster
{
public:
	Cluster(uint8_t* buffer, int size)
	{
		sys::io::byte_buffer bb((uint8_t*)buffer, 0, size);
	}
};

class DirEntry
{

};

class Fat
{
public:

};


int main(int argc, char* argv[])
{ 
  fstream ifs("FAT32.mdf"s);
  if (!ifs.good())
      cout << "ifs error";
  
  // super block
  char buffer_sb[0x200] = { 0 };
  ifs.read(buffer_sb, 0x200);
  byte_buffer bb_sb((uint8_t*)buffer_sb, 0, 0x200);
  Superblock sb((uint8_t*)buffer_sb, 0x200);
  //cout << sb.fat_size << endl;
  //cout << sb.cluster_size << endl;
  //cout << sb.fat_start_addr << endl;
  //cout << sb.fat_end_addr << endl;

  // data block
  char buffer_rd[0x1000] = { 0 };
  ifs.seekg(sb.fat_end_addr);
  ifs.read(buffer_rd, 0x1000);
  byte_buffer bb_rd((uint8_t*)buffer_rd, 0, 0x1000);
  Cluster root_dir((uint8_t*)buffer_rd, 0x1000);

  // fat block



  //vector<char> buffer2(sb.fat_size);
  //ifs.seekg(sb.resv_sector);
  //ifs.read(&buffer2[0], sb.fat_size);
  //byte_buffer bb2((uint8_t*)buffer2, 0, sb.fat_size);
  //Fatblock fb(buffer2, sb.fat_size);

  return 0;
}
