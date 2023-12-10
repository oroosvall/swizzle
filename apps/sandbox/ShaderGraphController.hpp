
#pragma once

#include <ImGuiSwizzleTools.hpp>

struct InputTemplate
{
    std::string mText;
    imext::NodeType mType;
    bool mPluggable;
};

struct OutputTemplate
{
    std::string mText;
    imext::NodeType mType;
};

struct NodeTemplate
{
    std::string mName;
    std::vector<InputTemplate> mInputs;
    std::vector<OutputTemplate> mOutputs;
};

class Node : public imext::ShaderNode
{
public:
    Node(NodeTemplate tmplate);
    virtual ~Node();

    virtual const char* getName() override;

    virtual const ImVec2& getPos() override;
    virtual void setPos(const ImVec2& pos) override;

    virtual const ImVec2& getSize() override;
    virtual void setSize(ImVec2& size) override;

    virtual std::vector<imext::Input>& getInputs() override;
    virtual std::vector<imext::Output>& getOutputs() override;

private:
    ImVec2 mPos;
    ImVec2 mSize;
    std::string mName;
    std::vector<imext::Input> mInputs;
    std::vector<imext::Output> mOutputs;
};

class InputNodeCollection : public imext::NodeCollection
{
public:
    InputNodeCollection();
    virtual ~InputNodeCollection();

    virtual const std::string& getName() const override;

    virtual U32 getNodeCount() const override;
    virtual const std::string& getNodeName(U32 index) const override;

    virtual std::shared_ptr<imext::ShaderNode> constructNode(U32 index) override;

private:
    std::string mName;
    std::string mInvalid;
    std::vector<NodeTemplate> mNodeTemplates;
};

class ShaderGraph : public imext::ShaderGraphController
{
public:
    ShaderGraph();
    virtual ~ShaderGraph();

    virtual std::vector<std::shared_ptr<imext::NodeCollection>>& getNodeCollection() override;

    virtual std::vector<std::shared_ptr<imext::ShaderNode>>& getNodes() override;

private:
    std::vector<std::shared_ptr<imext::NodeCollection>> mCollection;
    std::vector<std::shared_ptr<imext::ShaderNode>> mNodes;
};
