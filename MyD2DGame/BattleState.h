#pragma once

// Game Process State
enum class BattleState
{
    Explore, // Explore (Player)
	MoveToBattle, // Move to Battle (Transition Player,Enemy Region)
    ExpandBattle, // Battle Field Expanding
	Battle, // Battle (Player, Enemy)
    ReturnCenter, // 리턴하기전에(창이 작아지기전) 가운데로 resource복귀
    Return, // Return ( Rewind Expand Battle)
    ReturnExplore // Return to Explore
};