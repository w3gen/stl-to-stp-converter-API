#include <vector>
#include <string>
#include <math.h>
#include "StepKernel.h"
#include <iostream>
#include <fstream>

std::vector<double> read_stl_binary(std::string file_name)
{

	std::vector<double> nodes;
	std::ifstream file(file_name, std::ios::in | std::ios::binary);
	if (!file)
	{
		std::cout << "Failed to open binary stl file: " << file_name << "\n";
		return nodes;
	}

	char header[80];
	file.read(header, 80);

	uint32_t tris = 0;
	file.read((char*)(&tris),sizeof(uint32_t));
	nodes.resize(tris * 9);
	for (int i = 0; i < tris; i++)
	{
		float_t  n[3], pts[9];
		uint16_t att;
		file.read((char*)(n), sizeof(float_t) * 3);
		file.read((char*)(pts), sizeof(float_t) * 9);
		file.read((char*)(&att), sizeof(uint16_t));
		for (int j = 0; j < 9; j++)
			nodes[i * 9 + j] = pts[j];
	}
	file.close();
	return nodes;
}

std::vector<double> read_stl_ascii(std::string file_name)
{
	std::vector<double> nodes;
	std::ifstream file;
	file.open(file_name);
	if (!file)
	{
		std::cout << "Failed to open ascii stl file: " << file_name << "\n";
		return nodes;
	}
	std::string line;
	while (std::getline(file, line))
	{
		std::istringstream iss(line);
		std::string vert;
		double  x, y, z;
		if ((iss >> vert >> x >> y >> z) && vert == "vertex")
		{
			nodes.push_back(x);
			nodes.push_back(y);
			nodes.push_back(z);
		}
	}
	file.close();
	return nodes;
}

std::vector<double> read_stl(std::string file_name)
{
	// inspired by: https://stackoverflow.com/questions/26171521/verifying-that-an-stl-file-is-ascii-or-binary
	std::ifstream file_test(file_name, std::ifstream::ate | std::ifstream::binary);
	std::vector<double> nodes;
	if (!file_test)
	{
		std::cout << "Failed to open stl file: " << file_name << "\n";
		return nodes;
	}

	int file_size = file_test.tellg();
	file_test.close();

	// The minimum size of an empty ASCII file is 15 bytes.
	if (file_size < 15)
	{
		std::cout << "Invalid stl file: " << file_name << "\n";
		return nodes;
	}

	std::ifstream file(file_name, std::ios::in | std::ios::binary);
	if (!file)
	{
		std::cout << "Failed to open stl file: " << file_name << "\n";
		return nodes;
	}

	char first[5];
	file.read(first, 5);
	file.close();
	if (first[0] == 's' &&
		first[1] == 'o' &&
		first[2] == 'l' &&
		first[3] == 'i' &&
		first[4] == 'd')
	{
		nodes = read_stl_ascii(file_name);
	}
	else
	{
		nodes = read_stl_binary(file_name);
	}
	return nodes;
}

int main(int arv, char* argc[])
{
	double tol = 1e-6;
	bool mergeplanar = false;
	std::string help = "stltostp <stl_file> <step_file> [tol <value>] \n";

	if (arv < 3)
	{
		std::cout << "ERROR: " << help;
		return 1;
	}

	std::string input_file = argc[1];
	std::string output_file = argc[2];
	int arg_cnt = 3;
	while (arg_cnt < arv)
	{
		std::string cur_arg = argc[arg_cnt];
		if (cur_arg == "tol")
		{
			tol = std::atof(argc[arg_cnt + 1]);
			std::cout << "Minimum edge tolerance set to " << tol << "\n";
			arg_cnt++;
		}
		else if (cur_arg == "mergeplanar")
		{
			mergeplanar = true;
			std::cout << "Treating input file as a step file instead of stl...\n";
			arg_cnt++;
		}
		else
		{
			std::cout << "Unknown parameter " << cur_arg << "\n";
			return 1;
		}
		arg_cnt++;
	}

	std::vector<double> nodes = read_stl(input_file);
	if (nodes.size()/9 == 0)
	{
		std::cout << "No triangles found in stl file: " << input_file << "\n";
		return 1;
	}
	
	std::cout << "Read " << nodes.size() / 9 << " triangles from " << input_file << "\n";

	StepKernel se;
	int merged_edge_cnt = 0;
	se.build_tri_body(nodes,tol,merged_edge_cnt);
	se.write_step(output_file);
	std::cout << "Merged " << merged_edge_cnt << " edges\n";
	std::cout << "Exported STEP file: " << output_file << "\n";
	return 0;
}


