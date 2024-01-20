#include <iostream>
#include <fstream>
#include <vector>

#include "byte_buffer.hpp"

using namespace std;

class Node
{
public:
	string name;
	uint8_t attr;
	uint32_t cl_no;
	uint32_t clusters;
	uint32_t size;

	Node()
	{

	}

	auto expand()
	{

	}
};

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
	uint32_t fat_size;			// each fat
	uint32_t fat_start_addr;	// start fat region
	uint32_t fat_end_addr;		// start data region

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

class Fat
{
public:
	uint32_t entry_count;
	vector<uint32_t> fat0;

	Fat(uint8_t* buffer, int size)
	{
		entry_count = size / 4;
		sys::io::byte_buffer bb((uint8_t*)buffer, 0, size);
		for (int i = 0; i < entry_count; i++)
		{
			fat0.push_back(bb.get_uint32_le());
		}
	}
};

class DirEntry
{
public:
	string name;	// 0x00 제거 및 LFN일 때 합치는 것 주의
	string ext;
	uint8_t attr;
	uint16_t cl_high;
	uint16_t cl_low;
	uint32_t cl_no;
	uint32_t cl_end;
	uint32_t clusters;

	DirEntry(uint8_t* buffer, Fat fat)
	{
		sys::io::byte_buffer bb((uint8_t*)buffer, 0, 0x20);
		if (bb.compare_range(0x0B, 1, 0x0F))	// LFN 다음entry 읽어야 하지만 일단은 pass
		{
			bb.skip(0x01);
			name = bb.get_ascii(0x09);
			bb.skip(0x03);
			name += bb.get_ascii(0x0C);
			bb.skip(0x02);
			name += bb.get_ascii(0x04);
			attr = 0x0F;
		}
		else	// file or directory
		{
			name = bb.get_ascii(0x08);
			ext = bb.get_ascii(0x03);
			attr = bb.get_uint8();
			bb.skip(0x08);
			cl_high = bb.get_uint16_le();
			bb.skip(0x04);
			cl_low = bb.get_uint16_le();
			bb.skip(0x04);
			cl_no = (cl_high << 16) | cl_low;

			cl_end = cl_no;
			//while (fat.fat0[cl_end++] != 0x0fffffff);
			for (cl_end = cl_no; fat.fat0[cl_end] != 0x0fffffff; cl_end++);

			clusters = cl_end - cl_no + 1;
		}
	}

	auto to_node(Node node)
	{

	}

	auto export_to(uint8_t* buffer, string path, int size)	// file
	{
		std::ofstream output_file(path, std::ios::binary);
		if (!output_file.is_open()) {
			std::cerr << "Error: Unable to open file " << path << std::endl;
			return false;
		}
		output_file.write(reinterpret_cast<char*>(buffer), size);
		output_file.close();
		return true;
	}

	auto export_to(string path, int size) // dir
	{

	}
};

class Cluster
{
public:
	vector<DirEntry> entries;

	Cluster(std::ifstream& ifs, int size, uint32_t addr, Fat fat)
	{
		for (int offset = 0; offset < size; offset += 0x20)
		{
			char buffer[0x20] = { 0 };
			ifs.seekg(addr + offset);
			ifs.read(buffer, 0x20);
			DirEntry entry((uint8_t*)buffer, fat);
			entries.push_back(entry);
		}
	}

	Cluster(uint8_t* buffer, int size, Fat fat)
	{
		sys::io::byte_buffer bb((uint8_t*)buffer, 0, size);
		for (int offset = 0; offset < size; offset += 0x20)
		{
			uint8_t buffer_entry[0x20] = { 0 };
			memcpy(buffer_entry, &buffer[offset], 0x20);
			DirEntry entry(buffer_entry, fat);
			entries.push_back(entry);
		}
		int a = 0;
	}

	auto expand_all(std::ifstream& ifs, uint32_t data_area, Fat fat)
	{
		for (DirEntry entry : entries)
		{
			if (entry.attr == 10)		// directory
			{
				vector<uint8_t> buffer(0x1000 * entry.clusters);
				ifs.seekg((entry.cl_no - 2) * 0x1000 + data_area);
				ifs.read((char*)&buffer[0], 0x1000 * entry.clusters);
				Cluster dir(&buffer[0], 0x1000 * entry.clusters, fat);
				dir.expand_all(ifs, data_area, fat);
			}
			else if (entry.attr == 20)	// file
			{

			}
		}
	}
};

int main(int argc, char* argv[])
{ 
	ifstream ifs("C:/Users/bjh17/Downloads/forensic/FAT32_simple.mdf"s, ios_base::binary);
	if (!ifs.good())
		cout << "ifs error";
  
	// super block
	char buffer_sb[0x200] = { 0 };
	ifs.read(buffer_sb, 0x200);
	Superblock sb((uint8_t*)buffer_sb, 0x200);

	// fat block
	vector<uint8_t> b0(sb.fat_size);
	ifs.seekg(sb.fat_start_addr);
	ifs.read((char*) & b0[0], sb.fat_size);
	Fat fat(&b0[0], b0.size());

	// data block
	//leaf node
	char buffer_leaf[0x20] = { 0 };
	ifs.seekg(sb.fat_end_addr + 0x4040);
	ifs.read(buffer_leaf, 0x20);
	DirEntry leaf((uint8_t*)buffer_leaf, fat);

	// leaf content
	vector<uint8_t> leaf_content(leaf.clusters * 0x1000);
	ifs.seekg((leaf.cl_no - 2) * 0x1000 + sb.fat_end_addr);
	ifs.read((char*)&leaf_content[0], leaf.clusters * 0x1000);
	leaf.export_to(&leaf_content[0], "leaf.jpg", leaf.clusters * 0x1000);

	// root directory
	char buffer_rd[0x1000] = { 0 };
	ifs.seekg(sb.fat_end_addr);
	ifs.read(buffer_rd, 0x1000);
	Cluster rd((uint8_t*)buffer_rd, 0x1000, fat);
	//rd.expand(ifs, sb.fat_end_addr, fat);

	return 0;
}
