#include <DRAMPower/command/Command.h>
#include <DRAMPower/standards/ddr4/DDR4.h>
#include <DRAMPower/util/json.h>
#include "csv.hpp"
#include <DRAMPower/standards/ddr4/core_calculation_DDR4.h>
#include <DRAMPower/standards/ddr4/interface_calculation_DDR4.h>


#include <vector>
#include <filesystem>
#include <string_view>
#include <iostream>

using namespace DRAMPower;

std::vector<Command> parse_command_list(std::string_view csv_file)
{
	if (!std::filesystem::exists(csv_file) || std::filesystem::is_directory(csv_file))
	{
		std::cout<<"Error opening file: "<<csv_file<<std::endl;
		exit(1);
	}

	std::vector<Command> commandList;

	csv::CSVFormat format;
	format.no_header();
	format.trim({ ' ', '\t' });

	csv::CSVReader reader{ csv_file, format };
	for (csv::CSVRow& row : reader) { // Input iterator
		// timestamp, command, bank, bank group, rank
		auto cmdType = row[0].get_sv();
		auto timestamp = row[1].get<timestamp_t>();
		auto rank_id = row[2].get<std::size_t>();
		auto bank_id = row[3].get<std::size_t>();
		auto row_id = row[4].get<std::size_t>();
		auto bank_group_id = row[5].get<std::size_t>();
		auto column_id = row[6].get<std::size_t>();
		// std::size_t bank_group_id = 0;//row[3].get<std::size_t>();
		// std::size_t rank_id = 0;//row[4].get<std::size_t>();

		if (cmdType == "ACT") {
			commandList.push_back({ timestamp, CmdType::ACT, { bank_id, bank_group_id, rank_id } });
		}
		else if (cmdType == "PRE") {
			commandList.push_back({ timestamp, CmdType::PRE, { bank_id, bank_group_id, rank_id } });
		}
		else if (cmdType == "PREA") {
			commandList.push_back({ timestamp, CmdType::PREA, { bank_id, bank_group_id, rank_id } });
		}
		else if (cmdType == "PRESB") {
			commandList.push_back({ timestamp, CmdType::PRESB, { bank_id, bank_group_id, rank_id } });
		}
		else if (cmdType == "REFA" || cmdType == "REF") {
			commandList.push_back({ timestamp, CmdType::REFA, { bank_id, bank_group_id, rank_id } });
		}
		else if (cmdType == "REFB") {
			commandList.push_back({ timestamp, CmdType::REFB, { bank_id, bank_group_id, rank_id } });
		}
		else if (cmdType == "REFSB") {
			commandList.push_back({ timestamp, CmdType::REFSB, { bank_id, bank_group_id, rank_id } });
		}
		else if (cmdType == "RD") {
			commandList.push_back({ timestamp, CmdType::RD, { bank_id, bank_group_id, rank_id } });
		}
		else if (cmdType == "RDA") {
			commandList.push_back({ timestamp, CmdType::RDA, { bank_id, bank_group_id, rank_id } });
		}
		else if (cmdType == "WR") {
			commandList.push_back({ timestamp, CmdType::WR, { bank_id, bank_group_id, rank_id } });
		}
		else if (cmdType == "WRA") {
			commandList.push_back({ timestamp, CmdType::WRA, { bank_id, bank_group_id, rank_id } });
		}
		else if (cmdType == "PDEA") {
			commandList.push_back({ timestamp, CmdType::PDEA, { bank_id, bank_group_id, rank_id } });
		}
		else if (cmdType == "PDEP") {
			commandList.push_back({ timestamp, CmdType::PDEP, { bank_id, bank_group_id, rank_id } });
		}
		else if (cmdType == "PDXA") {
			commandList.push_back({ timestamp, CmdType::PDXA, { bank_id, bank_group_id, rank_id } });
		}
		else if (cmdType == "PDXP") {
			commandList.push_back({ timestamp, CmdType::PDXP, { bank_id, bank_group_id, rank_id } });
		}
		else if (cmdType == "SREN") {
			commandList.push_back({ timestamp, CmdType::SREFEN, { bank_id, bank_group_id, rank_id } });
		}
		else if (cmdType == "SREX") {
			commandList.push_back({ timestamp, CmdType::SREFEX, { bank_id, bank_group_id, rank_id } });
		}
		else if (cmdType == "DSMEN") {
			commandList.push_back({ timestamp, CmdType::DSMEN, { bank_id, bank_group_id, rank_id } });
		}
		else if (cmdType == "DSMEX") {
			commandList.push_back({ timestamp, CmdType::DSMEX, { bank_id, bank_group_id, rank_id } });
		}
		else if (cmdType == "END") {
			commandList.push_back({ timestamp, CmdType::END_OF_SIMULATION });
		};
	}

	return commandList;
};

int main(int argc, char *argv[])
{
	if (argc != 3) {
		std::cout<<"Usage: "<<argv[0]<<" <command_list.csv> <memspec.json>"<<std::endl;
		exit(1);
	}

	auto commandList = parse_command_list(argv[1]);
	std::cout<<"Parsed "<<commandList.size()<<" commands"<<std::endl;

	std::ifstream f((std::string(argv[2])));
	if (!f.is_open()) {
		std::cout<<"Error opening file: "<<argv[2]<<std::endl;
		exit(1);
	}

	json data = json::parse(f);
	DDR4 ddr(data["memspec"]);

	size_t count = 0;

	for (auto &command : commandList) {
		ddr.doCommand(command);
		count += 1;
	}

	auto energy = ddr.calcEnergy(commandList.back().timestamp);
	auto stats = ddr.getStats();

	std::cout << std::fixed;

	for (int b = 0; b < ddr.memSpec.numberOfBanks; b++) {
		std::cout<<b;
		std::cout<<" ACT:"<<energy.bank_energy[b].E_act;
		std::cout<<" PRE:"<<energy.bank_energy[b].E_pre;
		std::cout<<" RD:"<<energy.bank_energy[b].E_RD;
		std::cout<<" WR:"<<energy.bank_energy[b].E_WR;
		std::cout<<" RDA:"<<energy.bank_energy[b].E_RDA;
		std::cout<<" WRA:"<<energy.bank_energy[b].E_WRA;
		std::cout<<" REF:"<<energy.bank_energy[b].E_ref_AB;
		std::cout<<" BG_ACT:"<<energy.bank_energy[b].E_bg_act;
		std::cout<<" BG_PRE_"<<energy.bank_energy[b].E_bg_pre<<std::endl;
	}
	std::cout<<std::endl;
	// fmt::print("\n");
	std::cout<<"## E_bg_act: "<<energy.total_energy().E_bg_act<<std::endl;
	// fmt::print("## E_bg_act: {}\n", energy.total_energy().E_bg_act);
	std::cout<<"## E_bg_pre: "<<energy.total_energy().E_bg_pre<<std::endl;
	// fmt::print("## E_bg_pre: {}\n", energy.total_energy().E_bg_pre);
	std::cout<<"## E_bg_act_shared: "<<energy.E_bg_act_shared<<std::endl;
	// fmt::print("## E_bg_act_shared: {}\n", energy.E_bg_act_shared);
	std::cout<<"## E_PDNA: "<<energy.E_PDNA<<std::endl;
	// fmt::print("## E_PDNA: {}\n", energy.E_PDNA);
	std::cout<<"## E_PDNP: "<<energy.E_PDNP<<std::endl;
	// fmt::print("## E_PDNP: {}\n", energy.E_PDNP);
	// fmt::print("\n");
	std::cout<<std::endl;
	std::cout<<"Total Energy: "<<energy.total_energy().total() + energy.E_sref + energy.E_PDNA + energy.E_PDNP<<std::endl;
	// fmt::print("TOTAL ENERGY: {}\n", energy.total_energy().total() + energy.E_sref + energy.E_PDNA + energy.E_PDNP);

	return 0;
};