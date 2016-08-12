#ifndef ATLAS_HPP
#define ATLAS_HPP

#include "Math.hpp"
#include <memory>

namespace cyberCubes
{
namespace atlas
{

struct Tile
{
	math::ivec2 size, position;

	Tile(const math::ivec2 &size) : size(size), position(0, 0) {}
	Tile(const math::ivec2 &size, const math::ivec2 &position) : size(size), position(position) {}
};

class Atlas
{
public:
	Atlas(const math::ivec2 &size)
	{
		root = std::unique_ptr<Node>(new Node(size, math::ivec2(0, 0)));
	}

	const math::ivec2 &getSize() const
	{
		return root->rc.size;
	}

	bool insert(Tile &rc)
	{
		Node *node = insert(*root, rc);

		if (node)
		{
			node->hasImage = true;
			rc.position = node->rc.position;
			return true;
		}

		return false;
	}

private:
	struct Node
	{
		Node(const math::ivec2 &size, const math::ivec2 &position) : rc(size, position)
		{
			hasImage = false;
		}
		std::unique_ptr<Node> child[2];
		Tile rc;
		bool hasImage;
	};
	std::unique_ptr<Node> root;

	Node *insert(Node &node, const Tile &rc)
	{
		if (node.child[0]) // we're not a leaf
		{
			Node *newNode = insert(*node.child[0], rc);

			if (newNode)
				return newNode;

			return insert(*node.child[1], rc);
		}
		else
		{
			if (node.hasImage)
				return nullptr;

			if (node.rc.size.x < rc.size.x || node.rc.size.y < rc.size.y)
				return nullptr;

			if (node.rc.size == rc.size)
				return &node;

			int dw = node.rc.size.x - rc.size.x;
			int dh = node.rc.size.y - rc.size.y;

			if (dw > dh)
			{
				//	|------------|
				//	|     |      |
				//	|  rc |      |
				//	|     |      |
				//	|-----|      |
				//	|     |      |
				//	|------------|
				node.child[0].reset(new Node(math::ivec2(rc.size.x, node.rc.size.y), node.rc.position));
				node.child[1].reset(new Node(math::ivec2(dw, node.rc.size.y), node.rc.position + math::ivec2(rc.size.x, 0)));
			}
			else
			{
				//	|-----------|
				//	|  rc |     |
				//	|     |     |
				//	|-----|-----|
				//	|           |
				//	|           |
				//	|-----------|
				node.child[0].reset(new Node(math::ivec2(node.rc.size.x, rc.size.y), node.rc.position));
				node.child[1].reset(new Node(math::ivec2(node.rc.size.x, dh), node.rc.position + math::ivec2(0, rc.size.y)));
			}

			return insert(*node.child[0], rc);
		}
	}
};

}
}
#endif
