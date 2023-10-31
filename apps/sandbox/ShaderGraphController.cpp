
#include "ShaderGraphController.hpp"

Node::Node(ImVec2 pos, ImVec2 size)
    : mPos(pos)
    , mSize(size)
{
    imext::Input in{};
    in.mText = "Value";
    in.mPluggable = true;
    in.mInputType = imext::InputType::Float;
    mInputs.push_back(in);
    in.mInputType = imext::InputType::Vec2;
    mInputs.push_back(in);
    in.mInputType = imext::InputType::Vec3;
    mInputs.push_back(in);
    in.mInputType = imext::InputType::Vec4;
    mInputs.push_back(in);
    in.mInputType = imext::InputType::Color;
    mInputs.push_back(in);

    mOutputs.push_back({"value"});
    mOutputs.push_back({"test"});
    mOutputs.push_back({"bar"});
}

Node::~Node() {}

const ImVec2& Node::getPos()
{
    return mPos;
}

void Node::setPos(const ImVec2& pos)
{
    mPos = pos;
}

const ImVec2& Node::getSize()
{
    return mSize;
}

void Node::setSize(ImVec2& size)
{
    mSize = size;
}

std::vector<imext::Input>& Node::getInputs()
{
    return mInputs;
}

std::vector<imext::Output>& Node::getOutputs()
{
    return mOutputs;
}

ShaderGraph::ShaderGraph()
{
    mNodes.push_back(std::make_shared<Node>(ImVec2{0.0f, 0.0f}, ImVec2{100.0f, 30.0f}));
}

ShaderGraph::~ShaderGraph() {}

std::vector<imext::NodeCollection> ShaderGraph::getNodeCollection()
{
    return {};
}

std::vector<std::shared_ptr<imext::ShaderNode>> ShaderGraph::getNodes()
{
    return mNodes;
}
