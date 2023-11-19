
#include "ShaderGraphController.hpp"

Node::Node(ImVec2 pos, ImVec2 size, imext::NodeType type)
    : mPos(pos)
    , mSize(size)
{
    imext::Input in{};
    in.mText = "Value";
    in.mPluggable = true;
    in.mType = type;
    mInputs.push_back(in);

    mOutputs.push_back({"value", type});
    mOutputs.push_back({"test", type});
    mOutputs.push_back({"bar", type });
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

NodeCollectionThing::NodeCollectionThing(std::string name)
    : mName(name)
{
}

NodeCollectionThing::~NodeCollectionThing()
{
}

const std::string& NodeCollectionThing::getName() const
{
    return mName;
}


ShaderGraph::ShaderGraph()
{
    mCollection.push_back(std::make_shared<NodeCollectionThing>("Inputs"));
    mCollection.push_back(std::make_shared<NodeCollectionThing>("Outputs"));

    mNodes.push_back(std::make_shared<Node>(ImVec2{0.0f, 0.0f}, ImVec2{100.0f, 30.0f}, imext::NodeType::Float));
    mNodes.push_back(std::make_shared<Node>(ImVec2{0.0f, 0.0f}, ImVec2{100.0f, 30.0f}, imext::NodeType::Vec3));
    mNodes.push_back(std::make_shared<Node>(ImVec2{0.0f, 0.0f}, ImVec2{100.0f, 30.0f}, imext::NodeType::Color));
}

ShaderGraph::~ShaderGraph() {}

std::vector<std::shared_ptr<imext::NodeCollection>>& ShaderGraph::getNodeCollection()
{
    return mCollection;
}

std::vector<std::shared_ptr<imext::ShaderNode>>& ShaderGraph::getNodes()
{
    return mNodes;
}
