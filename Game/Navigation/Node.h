#pragma once

#include "Math/Vector2.h"

using namespace Wanted;

namespace Navigation
{

	struct Node
	{
	public:
		Node(const Vector2& position, Node* parentNode = nullptr)
			:position(position), parentNode(parentNode)
		{
		}


		bool operator==(const Node& other) const
		{
			return position.x == other.position.x
				&& position.y == other.position.y;
		}

	public:
		// The node's position.
		Vector2 position;

		// the cost from the start to this node. => Dijkstra.	
		float gCost = 0.0f;

		// the expected cost from this node to the Destination => Heuristic
		float hCost = 0.0f;

		// Final Cost (gCost + hCost)
		float fCost = 0.0f;

		// Link for the parent node.
		Node* parentNode = nullptr;
	};
}