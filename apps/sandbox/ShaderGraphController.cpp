
#include "ShaderGraphController.hpp"

Node::Node(NodeTemplate tmplate)
    : mPos(0.0f,0.f)
    , mSize(0.0f,0.f)
{
    for (auto& in : tmplate.mInputs)
    {
        imext::Input newIn{};
        newIn.mText = in.mText;
        newIn.mPluggable = in.mPluggable;
        newIn.mType = in.mType;
        mInputs.push_back(newIn);
    }
    for (auto& out : tmplate.mOutputs)
    {
        imext::Output newOut{};
        newOut.mText = out.mText;
        newOut.mType = out.mType;
        mOutputs.push_back(newOut);
    }
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

/// <summary>
///  Input nodes
/// </summary>

InputNodeCollection::InputNodeCollection()
    : mName("Inputs")
    , mInvalid("Invalid")
    , mNodeTemplates()
{
    mNodeTemplates.emplace_back(NodeTemplate{"Value",
                                             {InputTemplate{"value", imext::NodeType::Float, false}},
                                             {OutputTemplate{"Output", imext::NodeType::Float}}});
    mNodeTemplates.emplace_back(NodeTemplate{"Vector (2D)",
                                             {InputTemplate{"value", imext::NodeType::Vec2, false}},
                                             {OutputTemplate{"Output", imext::NodeType::Vec2}}});
    mNodeTemplates.emplace_back(NodeTemplate{"Vector (3D)",
                                             {InputTemplate{"value", imext::NodeType::Vec3, false}},
                                             {OutputTemplate{"Output", imext::NodeType::Vec3}}});
    mNodeTemplates.emplace_back(NodeTemplate{"Color",
                                             {InputTemplate{"value", imext::NodeType::Color, false}},
                                             {OutputTemplate{"Output", imext::NodeType::Color}}});
}
InputNodeCollection::~InputNodeCollection() {}

const std::string& InputNodeCollection::getName() const
{
    return mName;
}

U32 InputNodeCollection::getNodeCount() const
{
    return static_cast<U32>(mNodeTemplates.size());
}
const std::string& InputNodeCollection::getNodeName(U32 index) const
{
    if (index > mNodeTemplates.size())
    {
        return mInvalid;
    }
    return mNodeTemplates[index].mName;
}

std::shared_ptr<imext::ShaderNode> InputNodeCollection::constructNode(U32 index)
{
    if (index > mNodeTemplates.size())
    {
        return nullptr;
    }
    return std::make_shared<Node>(mNodeTemplates[index]);
}

ShaderGraph::ShaderGraph()
{
    mCollection.push_back(std::make_shared<InputNodeCollection>());
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
