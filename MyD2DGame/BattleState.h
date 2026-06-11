#pragma once

// Game Process State
enum class BattleState
{
    Explore, // Explore (Player)
	MoveToBattle, // Move to Battle (Transition Player,Enemy Region)
    ExpandBattle, // Battle Field Expanding
	Battle, // Battle (Player, Enemy)
    Return, // Return ( Rewind Expand Battle)
    ReturnExplore // Return to Explore
};