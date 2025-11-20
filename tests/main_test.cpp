#include <gtest/gtest.h>
#include <string>
#include <windows.h>
#include "TournamentBuilderAPI.h"
#include "matricesModule.h"
#include "Config.h"
#include "test_includes/utils.h"
#include "strat.h"
#include "TournamentConfig.h"
#include <chrono>
#include <iostream>
#include <print>



class CoreAlgorithmPerformance: public testing::TestWithParam<
    std::tuple<
        std::string, //players_path
        std::string, //ranks_path
        std::chrono::nanoseconds, //benchmark timing
        std::size_t, //n_players
        std::size_t  //rotation_size
        >>{};

const std::string base_dir="D:/VSCodeScripts/D/cpp_version/assets";
const std::string dummy_csv_path=base_dir+"/CorrettoOlly.csv";
const std::string dummy_rank_csv_path=base_dir+"/CorrettoOlly_rank.csv";

TEST_P(CoreAlgorithmPerformance, PerfTest){
    std::string csv_path=std::get<0>(GetParam());
    std::string ranks_csv_path= std::get<1>(GetParam());
    std::size_t n_players=std::get<3>(GetParam());
    std::size_t rotation_size=std::get<4>(GetParam());
    SetConsoleOutputCP(CP_UTF8); //changing windows console decoding to utf8
    std::println("Testing file \"{}\"", csv_path);
    CSVRows rows = readCSV(csv_path);
    ASSERT_NE(rows.size(), 0)
        <<"Could not open the players csv file or the file was empty...";
    CSVRows ranks_csv_rows= readCSV(ranks_csv_path);
    ASSERT_NE(ranks_csv_rows.size(), 0)
        <<"Could not open the ranks csv file or the file was empty...";
    ASSERT_EQ(initRankMaps(ranks_csv_rows), true)
        <<"Could not load ranks correctly, the ranks csv file was ill-formed...";
    PlayerList player_list = formatRowsToPlayerList(rows);
    ASSERT_NE(player_list.size(), 0)
        <<"Could not load players correctly, the players csv file was ill-formed...";
    

    int n_teams = static_cast<int>(player_list.size() / n_players);
    Config config=Config(n_teams, n_players);
    EXPECT_EQ(config.init(player_list), true)
        <<"Player list doesn't fit the teams' sizes... player list size: "<<player_list.size();
    std::chrono::duration elapsed= timeThis([&](){
        deduceAndApplyMoves(config, rotation_size);
    });
    //std::cout<<(std::string)TournamentConfig(config, player_list);
    std::chrono::nanoseconds expected=std::get<2>(GetParam());
    EXPECT_LE(elapsed, expected)<<elapsed<<" >= "<< expected;
}


INSTANTIATE_TEST_SUITE_P(
    TeamBuildingCoreAlgorithmTest,
    CoreAlgorithmPerformance,
    testing::Values(
        std::make_tuple(dummy_csv_path, dummy_rank_csv_path, std::chrono::milliseconds(500), 5, 3),
        std::make_tuple(dummy_csv_path, dummy_rank_csv_path, std::chrono::milliseconds(400), 5, 3),
        std::make_tuple(dummy_csv_path, dummy_rank_csv_path, std::chrono::milliseconds(300), 5, 3),
        std::make_tuple(dummy_csv_path, dummy_rank_csv_path, std::chrono::milliseconds(200), 5, 3),
        std::make_tuple(dummy_csv_path, dummy_rank_csv_path, std::chrono::milliseconds(100), 5, 3)
    )
);

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

class CoreAlgorithmDummy: public testing::TestWithParam<
    std::tuple<
        std::string, //players_path
        std::string, //ranks_path
        std::size_t, //n_players
        std::size_t  //rotation_size
        >>{};

TEST_P(CoreAlgorithmDummy, DummyTest){
    std::string csv_path=std::get<0>(GetParam());
    std::string ranks_csv_path= std::get<1>(GetParam());
    std::size_t n_players=std::get<2>(GetParam());
    std::size_t rotation_size=std::get<3>(GetParam());
    SetConsoleOutputCP(CP_UTF8); //changing windows console decoding to utf8
    std::println("Testing file \"{}\"", csv_path);
    CSVRows rows = readCSV(csv_path);
    ASSERT_NE(rows.size(), 0)
        <<"Could not open the players csv file or the file was empty...";
    CSVRows ranks_csv_rows= readCSV(ranks_csv_path);
    ASSERT_NE(ranks_csv_rows.size(), 0)
        <<"Could not open the ranks csv file or the file was empty...";
    ASSERT_EQ(initRankMaps(ranks_csv_rows), true)
        <<"Could not load ranks correctly, the ranks csv file was ill-formed...";
    PlayerList player_list = formatRowsToPlayerList(rows);
    ASSERT_NE(player_list.size(), 0)
        <<"Could not load players correctly, the players csv file was ill-formed...";
    
    int n_teams = static_cast<int>(player_list.size() / n_players);
    Config config=Config(n_teams, n_players);
    ASSERT_EQ(config.init(player_list), true)
        <<"Player list doesn't fit the teams' sizes... player list size: "<<player_list.size();
    std::chrono::duration elapsed= timeThis([&](){
        deduceAndApplyMoves(config, rotation_size);
    });
    //std::cout<<(std::string)TournamentConfig(config, player_list);
    EXPECT_EQ(true, true)<<"like... how did this fail?";
}


INSTANTIATE_TEST_SUITE_P(
    TeamBuildingCoreAlgorithmTest,
    CoreAlgorithmDummy,
    testing::Values(
        std::make_tuple(dummy_csv_path, dummy_rank_csv_path, 5, 3),
        std::make_tuple(
            dummy_csv_path, 
            base_dir+"/CorrettoOlly_rank_v2.csv", 
            5, 3
        ),
        std::make_tuple(
            base_dir+"/Torneo_itsme_-_Edizione_1.csv",
            base_dir+"/Itsme_Ed1_rank.csv", 
            5, 3
        ),
        std::make_tuple(
            base_dir+"/Torneo_itsme_-_Edizione_2_polished.csv",
            base_dir+"/Itsme_Ed2_rank.csv", 
            5, 3
        ),
        std::make_tuple(
            base_dir+"/Torneo_itsme_-_Edizione_2_mod.csv",
            base_dir+"/Itsme_Ed2_mod_rank.csv", 
            5, 3
        )
    )
);


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

