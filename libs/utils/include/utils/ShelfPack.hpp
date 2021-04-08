#ifndef SHELFPACK_HPP
#define SHELFPACK_HPP

#include <cstdint>
#include <vector>

struct PackRect {
	int32_t x, y;
	int32_t w, h;

	int32_t id;
};

class ShelfPack {
public:

	ShelfPack(int32_t x, int32_t y);
	~ShelfPack();

	bool addData(int32_t w, int32_t h, int32_t id);

	size_t length();

	int32_t getPackedWidth();
	int32_t getPackedHeight();

	PackRect operator[](int32_t id);

private:

	struct Node {
		int32_t x;
		int32_t y;
		int32_t height;
		int32_t width;

		int32_t id;

		Node* side;
		Node* below;

		bool insert(Node* n, int32_t maxX, int32_t maxY);

		int32_t getHeight();
		int32_t getWidth();
	};

	bool addNode(Node* n);

	int32_t width;
	int32_t height;

	size_t counter;

	Node* root;

	std::vector<Node*> nodes;
};

#endif