
#pragma once

#include <ImGuiSwizzleTools.hpp>

class Node : public imext::ShaderNode
{
public:
    Node(ImVec2 pos, ImVec2 size, imext::NodeType type);
    virtual ~Node();

    virtual const ImVec2& getPos() override;
    virtual void setPos(const ImVec2& pos) override;

    virtual const ImVec2& getSize() override;
    virtual void setSize(ImVec2& size) override;

    virtual std::vector<imext::Input>& getInputs() override;
    virtual std::vector<imext::Output>& getOutputs() override;

private:
    ImVec2 mPos;
    ImVec2 mSize;
    std::vector<imext::Input> mInputs;
    std::vector<imext::Output> mOutputs;
};

class NodeCollectionThing : public imext::NodeCollection
{
public:
    NodeCollectionThing(std::string name);
    virtual ~NodeCollectionThing();

    virtual const std::string& getName() const override;

private:
    std::string mName;
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
