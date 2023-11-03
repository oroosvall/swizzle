
/* Include files */

#include <ImGuiSwizzleTools.hpp>

#include <imgui/imgui_internal.h>

/* Defines */

constexpr ImGuiID SCROLL_X_LOC = 0;
constexpr ImGuiID SCROLL_Y_LOC = 1;
constexpr ImGuiID SELECTED_NODE = 2;
constexpr ImGuiID LINK_START_TYPE = 3;     // 0 for input, 1 for output, -1 not dragging
constexpr ImGuiID LINK_START_NODE_IDX = 4; // Node index for operattion, input/output
constexpr ImGuiID LINK_START_LNK_IDX = 5;  // Index for input/output link

constexpr int INPUT_START_TYPE = 0;
constexpr int OUTPUT_START_TYPE = 1;

/* Typedefs */

enum class PipelineShape
{
    Box,
    Begin,
    Middle,
    End
};

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs);
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs);

static inline ImVec2 GetNodeInputPos(imext::ShaderNode* node, size_t idx);
static inline ImVec2 GetNodeOutputPos(imext::ShaderNode* node, size_t idx);

static inline ImVec2 DrawShapeWithText(ImVec2 topLeft, PipelineShape shape, const char* text);
static inline void RenderPipeline();

static inline void DrawNodeInput(imext::Input& in);

/* Static Function Definition */

static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs)
{
    return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y);
}

static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs)
{
    return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y);
}

static inline ImVec2 GetNodeInputPos(imext::ShaderNode* node, size_t idx)
{
    ImVec2 pos = node->getPos();
    ImVec2 size = node->getSize();
    ImVec2 inPos = node->getInputs()[idx].mPos;
    ImVec2 inSize = node->getInputs()[idx].mSize;
    return ImVec2(pos.x, inPos.y);
}

static inline ImVec2 GetNodeOutputPos(imext::ShaderNode* node, size_t idx)
{
    ImVec2 pos = node->getPos();
    ImVec2 size = node->getSize();
    ImVec2 outPos = node->getOutputs()[idx].mPos;
    ImVec2 outSize = node->getOutputs()[idx].mSize;
    return ImVec2(pos.x + size.x, outPos.y);
}

static inline void AddPolyFilled(ImDrawList* draw_list, const ImVec2* points, int points_count,
                                 const uint32_t* indicies, int index_count, ImU32 color)
{
    if (points_count < 3)
        return;

    const ImVec2 uv = draw_list->_Data->TexUvWhitePixel;

    draw_list->PrimReserve(index_count, points_count);

    // TODO: anti-aliasing?

    for (int i = 0; i < points_count; i++)
    {
        draw_list->_VtxWritePtr[0].pos = points[i];
        draw_list->_VtxWritePtr[0].uv = uv;
        draw_list->_VtxWritePtr[0].col = color;
        draw_list->_VtxWritePtr++;
    }

    for (int i = 0; i < index_count; i += 3)
    {
        draw_list->_IdxWritePtr[0] = static_cast<unsigned short>(draw_list->_VtxCurrentIdx + indicies[i]);
        draw_list->_IdxWritePtr[1] = static_cast<unsigned short>(draw_list->_VtxCurrentIdx + indicies[i + 1]);
        draw_list->_IdxWritePtr[2] = static_cast<unsigned short>(draw_list->_VtxCurrentIdx + indicies[i + 2]);
        draw_list->_IdxWritePtr += 3;
    }

    draw_list->_VtxCurrentIdx += points_count;
}

static inline ImVec2 DrawShapeWithText(ImVec2 topLeft, PipelineShape shape, const char* text)
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 textSize = ImGui::CalcTextSize(text);
    float textMargin = 10.0f;
    ImVec2 arrow = {25.0f, 25.0f};
    float height = arrow.y * 2.0f;

    const ImVec2 p = topLeft;

    ImVec2 box[] = {p,
                    {p.x + textSize.x + (textMargin * 2.0f), p.y + 0.0f},
                    {p.x + textSize.x + (textMargin * 2.0f), p.y + height},
                    {p.x + 0.0f, p.y + height}};

    ImVec2 begin[] = {p,
                      {p.x + textSize.x + textMargin, p.y + 0.0f},
                      {p.x + textSize.x + textMargin + arrow.x, p.y + arrow.y},
                      {p.x + textSize.x + textMargin, p.y + height},
                      {p.x + 0.0f, p.y + height}};

    ImVec2 middle[] = {
        p,
        {p.x + textSize.x + textMargin + arrow.x, p.y + 0.0f},
        {p.x + textSize.x + textMargin + arrow.x, p.y + height},
        {p.x + 0.0f, p.y + height},
        {p.x + arrow.x, p.y + arrow.y},
        {p.x + textSize.x + textMargin + (arrow.x * 2.0f), p.y + arrow.y},
    };
    uint32_t middleBox[] = {0, 1, 4, 1, 2, 4, 2, 3, 4, 1, 5, 2};

    ImVec2 end[] = {
        p,
        {p.x + textSize.x + (textMargin * 2.0f) + arrow.x, p.y + 0.0f},
        {p.x + textSize.x + (textMargin * 2.0f) + arrow.x, p.y + height},
        {p.x + 0.0f, p.y + height},
        {p.x + arrow.x, p.y + arrow.y},
    };
    uint32_t endBox[] = {0, 1, 4, 1, 2, 4, 2, 3, 4};

    ImVec2 newPos = {};

    float textY = arrow.y - (textSize.y / 2.0f);

    switch (shape)
    {
        break;
    case PipelineShape::Begin:
        draw_list->AddConvexPolyFilled(begin, COUNT_OF(begin), IM_COL32(50, 220, 125, 255));
        draw_list->AddText({p.x + textMargin, p.y + textY}, IM_COL32(0, 0, 0, 255), text);
        newPos = {p.x + textSize.x + textMargin, p.y};
        break;
    case PipelineShape::Middle:
        AddPolyFilled(draw_list, middle, COUNT_OF(middle), middleBox, COUNT_OF(middleBox), IM_COL32(50, 220, 125, 255));
        draw_list->AddText({p.x + textMargin + arrow.x, p.y + textY}, IM_COL32(0, 0, 0, 255), text);
        newPos = {p.x + textSize.x + textMargin + arrow.x, p.y};
        break;
    case PipelineShape::End:
        AddPolyFilled(draw_list, end, COUNT_OF(end), endBox, COUNT_OF(endBox), IM_COL32(50, 220, 125, 255));
        draw_list->AddText({p.x + textMargin + arrow.x, p.y + textY}, IM_COL32(0, 0, 0, 255), text);
        newPos = {p.x + textSize.x + (textMargin * 2.0f) + arrow.x, p.y};
        break;
    case PipelineShape::Box:
    default:
        draw_list->AddConvexPolyFilled(box, COUNT_OF(box), IM_COL32(50, 220, 125, 255));
        draw_list->AddText({p.x + textMargin, p.y + textY}, IM_COL32(0, 0, 0, 255), text);
        newPos = {p.x + textSize.x + textMargin, p.y};
        break;
    }

    newPos.x += 2;

    return newPos;
}

static inline void RenderPipeline()
{
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImVec2 s = p;
    p = DrawShapeWithText(p, PipelineShape::Begin, "Vertex");
    p = DrawShapeWithText(p, PipelineShape::Middle, "Tess Ctrl");
    p = DrawShapeWithText(p, PipelineShape::Middle, "Tess Eval");
    p = DrawShapeWithText(p, PipelineShape::Middle, "Geometry");
    p = DrawShapeWithText(p, PipelineShape::End, "Fragment");
    p = DrawShapeWithText(p, PipelineShape::Box, "Compute");
    p.x += 10.0f;
    ImGui::ItemSize(p - s);
}

static inline void DrawNodeInput(imext::Input& in)
{
    ImGui::PushID(&in);
    in.mPos = ImGui::GetCursorScreenPos();
    if (!in.mText.empty())
    {
        ImGui::Text("  %s", in.mText.c_str());
    }
    in.mSize = ImGui::GetItemRectSize();
    if (in.mSource.lock())
    {
        goto exit;
    }
    if (in.mInputType == imext::InputType::Float)
    {
        ImGui::DragFloat("##value", in.mInputData);
    }
    else if (in.mInputType == imext::InputType::Vec2)
    {
        ImGui::BeginGroup();
        ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_ItemSpacing, {0, 1});
        ImGui::DragFloat("##value1", in.mInputData);
        ImGui::DragFloat("##value2", in.mInputData + 1);
        ImGui::PopStyleVar();
        ImGui::EndGroup();
    }
    else if (in.mInputType == imext::InputType::Vec3)
    {
        ImGui::BeginGroup();
        ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_ItemSpacing, {0, 1});
        ImGui::DragFloat("##value1", in.mInputData);
        ImGui::DragFloat("##value2", in.mInputData + 1);
        ImGui::DragFloat("##value3", in.mInputData + 2);
        ImGui::PopStyleVar();
        ImGui::EndGroup();
    }
    else if (in.mInputType == imext::InputType::Vec4)
    {
        ImGui::BeginGroup();
        ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_ItemSpacing, {0, 1});
        ImGui::DragFloat("##value1", in.mInputData);
        ImGui::DragFloat("##value2", in.mInputData + 1);
        ImGui::DragFloat("##value3", in.mInputData + 2);
        ImGui::DragFloat("##value4", in.mInputData + 3);
        ImGui::PopStyleVar();
        ImGui::EndGroup();
    }
    else if (in.mInputType == imext::InputType::Color)
    {
        ImGui::BeginGroup();
        imext::ColorPickerbutton4("##value", in.mInputData, ImGuiColorEditFlags_::ImGuiColorEditFlags_DataTypeMask_);
        ImGui::EndGroup();
    }
    exit:
    ImGui::PopID();
}

/* Function Definition */

namespace imext
{
#if 0
    void ShaderGraph(const char* name, bool* open, ShaderGraphController* sgc)
    {
        ImGui::SetNextWindowSize(ImVec2(700, 600), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin(name, open))
        {
            ImGui::End();
            return;
        }

        ImGuiIO& io = ImGui::GetIO();

        ImGuiStorage* storage = ImGui::GetStateStorage();
        ImVec2 scrolling = ImVec2(storage->GetFloat(SCROLL_X_LOC), storage->GetFloat(SCROLL_Y_LOC));
        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        ImGui::BeginChild("pipeline", ImVec2(0.0f, 75.0f), true, ImGuiWindowFlags_HorizontalScrollbar);
        RenderPipeline();
        ImGui::EndChild();

        ImGui::BeginGroup();

        const float NODE_SLOT_RADIUS = 4.0f;
        const ImVec2 NODE_WINDOW_PADDING(8.0f, 8.0f);

        ImGui::Text("Hold middle mouse button to scroll (%.2f,%.2f)", scrolling.x, scrolling.y);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(60, 60, 70, 200));
        ImGui::BeginChild("scrolling_region", ImVec2(0, 0), true,
                          ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);
        ImGui::PushItemWidth(120.0f);
        ImGui::PopStyleVar();

        const ImVec2 offset = ImGui::GetCursorScreenPos() + scrolling;

        // Display grid
        if (true)
        {
            ImU32 GRID_COLOR = IM_COL32(200, 200, 200, 40);
            float GRID_SZ = 64.0f;
            ImVec2 win_pos = ImGui::GetCursorScreenPos();
            ImVec2 canvas_sz = ImGui::GetWindowSize();
            for (float x = fmodf(scrolling.x, GRID_SZ); x < canvas_sz.x; x += GRID_SZ)
                draw_list->AddLine(ImVec2(x, 0.0f) + win_pos, ImVec2(x, canvas_sz.y) + win_pos, GRID_COLOR);
            for (float y = fmodf(scrolling.y, GRID_SZ); y < canvas_sz.y; y += GRID_SZ)
                draw_list->AddLine(ImVec2(0.0f, y) + win_pos, ImVec2(canvas_sz.x, y) + win_pos, GRID_COLOR);
        }

        int node_hovered_in_scene = -1;
        int selectedNode = storage->GetInt(SELECTED_NODE, -1);

        int linkDragMode = storage->GetInt(LINK_START_TYPE, -1);
        int linkNodeIndex = storage->GetInt(LINK_START_NODE_IDX, -1);
        int linkNodeLnkIndex = storage->GetInt(LINK_START_LNK_IDX, -1);
        bool completed = false;

        // Display links
        draw_list->AddDrawCmd();
        draw_list->ChannelsSplit(2);
        draw_list->ChannelsSetCurrent(0);

        std::vector<ShaderLink> links;

        std::vector<std::shared_ptr<ShaderNode>>& nodes = sgc->getNodes();
        for (size_t i = 0; i < nodes.size(); ++i)
        {
            std::shared_ptr<ShaderNode> node = nodes[i];
            ImGui::PushID(i);
            ImVec2 node_rect_min = offset + node->getPos();

            draw_list->ChannelsSetCurrent(1); // Foreground
            bool old_any_active = ImGui::IsAnyItemActive();
            ImGui::SetCursorScreenPos(node_rect_min + NODE_WINDOW_PADDING);
            // ImVec2 pPos = ImGui::GetCursorScreenPos();
            ImGui::BeginGroup(); // Lock horizontal position
            ImGui::Text("%s", "test");
            //ImGui::BeginGroup();
            //for (auto& in : node->getInputs())
            //{
            //    ImGui::PushID(&in);
            //    in.mPos = ImGui::GetCursorScreenPos();
            //    if (!in.mText.empty())
            //    {
            //        ImGui::Text("  %s", in.mText.c_str());
            //    }
            //    in.mSize = ImGui::GetItemRectSize();

            //    if (auto src = in.mSource.lock())
            //    {
            //        ShaderLink lnk;
            //        lnk.mEndPos = in.mPos;
            //        lnk.mEndPos.x += offset.x;
            //        lnk.mEndPos.y += (in.mSize.y / 2.0f);

            //        lnk.mStartPos = src->getPos();
            //        lnk.mStartPos.x += offset.x;
            //        lnk.mStartPos.y += (src->getSize().y / 2.0f);
            //        links.push_back(lnk);
            //    }

            //    if (in.mInputType == imext::InputType::Float)
            //    {
            //        ImGui::DragFloat("##value", in.mInputData);
            //    }
            //    else if (in.mInputType == imext::InputType::Vec2)
            //    {
            //        ImGui::BeginGroup();
            //        ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_ItemSpacing, {0, 1});
            //        ImGui::DragFloat("##value1", in.mInputData);
            //        ImGui::DragFloat("##value2", in.mInputData + 1);
            //        ImGui::PopStyleVar();
            //        ImGui::EndGroup();
            //    }
            //    else if (in.mInputType == imext::InputType::Vec3)
            //    {
            //        ImGui::BeginGroup();
            //        ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_ItemSpacing, {0, 1});
            //        ImGui::DragFloat("##value1", in.mInputData);
            //        ImGui::DragFloat("##value2", in.mInputData + 1);
            //        ImGui::DragFloat("##value3", in.mInputData + 2);
            //        ImGui::PopStyleVar();
            //        ImGui::EndGroup();
            //    }
            //    else if (in.mInputType == imext::InputType::Vec4)
            //    {
            //        ImGui::BeginGroup();
            //        ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_ItemSpacing, {0, 1});
            //        ImGui::DragFloat("##value1", in.mInputData);
            //        ImGui::DragFloat("##value2", in.mInputData + 1);
            //        ImGui::DragFloat("##value3", in.mInputData + 2);
            //        ImGui::DragFloat("##value4", in.mInputData + 3);
            //        ImGui::PopStyleVar();
            //        ImGui::EndGroup();
            //    }
            //    else if (in.mInputType == imext::InputType::Color)
            //    {
            //        ImGui::BeginGroup();
            //        ColorPickerbutton4("##value", in.mInputData, ImGuiColorEditFlags_::ImGuiColorEditFlags_DataTypeMask_);
            //        ImGui::EndGroup();
            //    }

            //    ImGui::PopID();
            //    // draw_list->AddRect(in.mPos, in.mPos + in.mSize, IM_COL32(255, 255, 255, 255));
            //}
            //ImGui::EndGroup();
            //ImGui::SameLine();
            //ImGui::BeginGroup();
            //ImGui::NewLine();
            //for (auto& out : node->getOutputs())
            //{
            //    out.mPos = ImGui::GetCursorScreenPos();
            //    ImGui::Text("%s", out.mText.c_str());
            //    out.mSize = ImGui::GetItemRectSize();
            //}
            //// ImGui::SliderFloat("##value", &node->Value, 0.0f, 1.0f, "Alpha %.2f");
            //// ImGui::ColorEdit3("##color", &node->Color.x);
            //ImGui::EndGroup();
            ImGui::EndGroup();

            // Save the size of what we have emitted and whether any of the widgets are being used
            bool node_widgets_active = (!old_any_active && ImGui::IsAnyItemActive());
            auto size = ImGui::GetItemRectSize() + NODE_WINDOW_PADDING + NODE_WINDOW_PADDING;
            node->setSize(size);
            ImVec2 node_rect_max = node_rect_min + node->getSize();

            // Display node box
            draw_list->ChannelsSetCurrent(0); // Background
            ImGui::SetCursorScreenPos(node_rect_min);
            ImGui::InvisibleButton("node", node->getSize());
            if (ImGui::IsItemHovered())
            {
                node_hovered_in_scene = i;
                // open_context_menu |= ImGui::IsMouseClicked(1);
            }
            bool node_moving_active = ImGui::IsItemActive();
            if (node_widgets_active || node_moving_active)
            {
                selectedNode = i;
            }
            if (node_moving_active && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
            {
                ImVec2 pos = node->getPos() + io.MouseDelta;
                node->setPos(pos);
            }

            ImU32 node_bg_color = (node_hovered_in_scene == i || (selectedNode == i)) ? IM_COL32(75, 75, 75, 255)
                                                                                      : IM_COL32(60, 60, 60, 255);
            draw_list->AddRectFilled(node_rect_min, node_rect_max, node_bg_color, 4.0f);
            draw_list->AddRect(node_rect_min, node_rect_max, IM_COL32(100, 100, 100, 255), 4.0f);

            auto& inputs = node->getInputs();
            for (int slot_idx = 0; slot_idx < inputs.size(); slot_idx++)
            {
                if (inputs[slot_idx].mPluggable)
                {
                    ImVec2 pos = GetNodeInputPos(node.get(), slot_idx);
                    ImVec2 size = inputs[slot_idx].mSize;
                    ImVec2 nPos = {};
                    nPos.x = offset.x + pos.x;
                    nPos.y = pos.y + (size.y / 2.0f);
                    draw_list->AddCircleFilled(nPos, NODE_SLOT_RADIUS, IM_COL32(150, 150, 150, 150));
                    //draw_list->ChannelsSetCurrent(1);
                    //ImGui::PushID(slot_idx);
                    //ImGui::SetCursorScreenPos(nPos);
                    //ImGui::Button("##inputLink", size);
                    //if (ImGui::BeginDragDropSource())
                    //{
                    //    linkDragMode = INPUT_START_TYPE;
                    //    linkNodeIndex = i;
                    //    linkNodeLnkIndex = slot_idx;
                    //    ImGui::SetDragDropPayload("INPUT_LINK", nullptr, 0);
                    //    ImGui::EndDragDropSource();
                    //}
                    //if (ImGui::BeginDragDropTarget() && linkDragMode == OUTPUT_START_TYPE)
                    //{
                    //    if (ImGui::AcceptDragDropPayload("OUTPUT_LINK"))
                    //    {
                    //        inputs[slot_idx].mSource = nodes[linkNodeIndex];
                    //        inputs[slot_idx].mSourceIndex = linkNodeLnkIndex;
                    //    }
                    //    ImGui::EndDragDropTarget();
                    //}
                    //ImGui::PopID();
                    //draw_list->ChannelsSetCurrent(0);
                }
            }
            auto& outputs = node->getOutputs();
            for (int slot_idx = 0; slot_idx < outputs.size(); slot_idx++)
            {
                ImVec2 pos = GetNodeOutputPos(node.get(), slot_idx);
                ImVec2 size = outputs[slot_idx].mSize;
                ImVec2 nPos = {};
                nPos.x = offset.x + pos.x;
                nPos.y = pos.y + (size.y / 2.0f);
                draw_list->AddCircleFilled(nPos, NODE_SLOT_RADIUS, IM_COL32(150, 150, 150, 150));
                //draw_list->ChannelsSetCurrent(1);
                //ImGui::PushID(slot_idx);
                //ImGui::SetCursorScreenPos(nPos);
                //ImGui::Button("##outputLink", size);
                //if (ImGui::BeginDragDropSource())
                //{
                //    linkDragMode = OUTPUT_START_TYPE;
                //    linkNodeIndex = i;
                //    linkNodeLnkIndex = slot_idx;
                //    ImGui::SetDragDropPayload("OUTPUT_LINK", nullptr, 0);
                //    ImGui::EndDragDropSource();
                //}
                //if (ImGui::BeginDragDropTarget() && linkDragMode == INPUT_START_TYPE)
                //{
                //    if (ImGui::AcceptDragDropPayload("INPUT_LINK"))
                //    {
                //        auto& inputNode = nodes[linkNodeIndex];
                //        auto& input = inputNode->getInputs()[linkNodeIndex];
                //        input.mSource = node;
                //        input.mSourceIndex = slot_idx;
                //    }
                //    ImGui::EndDragDropTarget();
                //}
                //ImGui::PopID();
                //draw_list->ChannelsSetCurrent(0);
            }

            ImGui::PopID();
        }

        if (linkDragMode == INPUT_START_TYPE)
        {
            auto& node = nodes[linkNodeIndex];
            ShaderLink lnk{};
            lnk.mStartPos = GetNodeInputPos(node.get(), linkNodeLnkIndex);
            lnk.mStartPos.x += offset.x;
            lnk.mStartPos.y += (node->getSize().y / 2.0f);

            lnk.mEndPos = ImGui::GetMousePos();
            links.push_back(lnk);
        }
        else if (linkDragMode == OUTPUT_START_TYPE)
        {
            auto& node = nodes[linkNodeIndex];

            ShaderLink lnk{};
            lnk.mStartPos = GetNodeOutputPos(node.get(), linkNodeLnkIndex);
            lnk.mStartPos.x += offset.x;
            lnk.mStartPos.y += (node->getSize().y / 2.0f);

            lnk.mEndPos = ImGui::GetMousePos();
            links.push_back(lnk);
        }
        else
        {
            // do nothing
        }

        draw_list->ChannelsSetCurrent(0); // Background
        for (auto& link : links)
        {
            ImVec2 startPos = link.mStartPos;
            ImVec2 endPos = link.mEndPos;
            draw_list->AddBezierCubic(startPos, startPos + ImVec2(+50, 0), endPos + ImVec2(-50, 0), endPos, IM_COL32(200, 200, 100, 255), 3.0f);
        }

        draw_list->ChannelsMerge();

        if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() &&
            ImGui::IsMouseDragging(ImGuiMouseButton_Middle, 0.0f))
        {
            scrolling = scrolling + io.MouseDelta;
            storage->SetFloat(SCROLL_X_LOC, scrolling.x);
            storage->SetFloat(SCROLL_Y_LOC, scrolling.y);
            storage->SetInt(SELECTED_NODE, selectedNode);
            storage->SetInt(LINK_START_TYPE, linkDragMode);
            storage->SetInt(LINK_START_NODE_IDX, linkNodeIndex);
            storage->SetInt(LINK_START_LNK_IDX, linkNodeLnkIndex);
        }

        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
        ImGui::EndGroup();

        ImGui::End();
    }

#endif

#if 1

    void ShaderGraph(const char* name, bool* open, ShaderGraphController* sgc)
    {
        ImGui::SetNextWindowSize(ImVec2(700, 600), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin(name, open))
        {
            ImGui::End();
            return;
        }

        // State
        static bool show_grid = true;

        // Initialization
        ImGuiIO& io = ImGui::GetIO();
        ImGuiStorage* storage = ImGui::GetStateStorage();
        ImVec2 scrolling = ImVec2(storage->GetFloat(SCROLL_X_LOC), storage->GetFloat(SCROLL_Y_LOC));
        int selectedNode = storage->GetInt(SELECTED_NODE, -1);
        int linkDragMode = storage->GetInt(LINK_START_TYPE, -1);
        int linkNodeIndex = storage->GetInt(LINK_START_NODE_IDX, -1);
        int linkNodeLnkIndex = storage->GetInt(LINK_START_LNK_IDX, -1);

        // Draw a list of nodes on the left side
        int node_hovered_in_scene = -1;
        ImGui::BeginChild("pipeline", ImVec2(0.0f, 75.0f), true, ImGuiWindowFlags_HorizontalScrollbar);
        RenderPipeline();
        ImGui::EndChild();

        ImGui::BeginGroup();

        const float NODE_SLOT_RADIUS = 4.0f;
        const ImVec2 NODE_WINDOW_PADDING(8.0f, 8.0f);

        // Create our child canvas
        ImGui::Text("Hold middle mouse button to scroll (%.2f,%.2f)", scrolling.x, scrolling.y);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(60, 60, 70, 200));
        ImGui::BeginChild("scrolling_region", ImVec2(0, 0), true,
                          ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse);
        ImGui::PopStyleVar(); // WindowPadding
        ImGui::PushItemWidth(120.0f);

        const ImVec2 offset = ImGui::GetCursorScreenPos() + scrolling;
        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        // Display grid
        if (show_grid)
        {
            ImU32 GRID_COLOR = IM_COL32(200, 200, 200, 40);
            float GRID_SZ = 64.0f;
            ImVec2 win_pos = ImGui::GetCursorScreenPos();
            ImVec2 canvas_sz = ImGui::GetWindowSize();
            for (float x = fmodf(scrolling.x, GRID_SZ); x < canvas_sz.x; x += GRID_SZ)
                draw_list->AddLine(ImVec2(x, 0.0f) + win_pos, ImVec2(x, canvas_sz.y) + win_pos, GRID_COLOR);
            for (float y = fmodf(scrolling.y, GRID_SZ); y < canvas_sz.y; y += GRID_SZ)
                draw_list->AddLine(ImVec2(0.0f, y) + win_pos, ImVec2(canvas_sz.x, y) + win_pos, GRID_COLOR);
        }

        // Display links

        // Display nodes
        std::vector<ShaderLink> links;

        std::vector<std::shared_ptr<ShaderNode>>& nodes = sgc->getNodes();
        int channelCount = (2 * static_cast<int>(nodes.size())) + 1;

        draw_list->ChannelsSplit(channelCount);
        bool newActive = false;
        bool isHover = false;

        for (size_t i = 0ull; i < nodes.size(); ++i)
        {
            size_t idx = nodes.size() - i - 1ull;
            std::shared_ptr<ShaderNode> node = nodes[idx];
            ImGui::PushID(node.get());
            ImVec2 node_rect_min = offset + node->getPos();

            draw_list->ChannelsSetCurrent(static_cast<int>(idx)*2 + 1); // Foreground
            bool old_any_active = ImGui::IsAnyItemActive();
            ImGui::SetCursorScreenPos(node_rect_min + NODE_WINDOW_PADDING);
            // ImVec2 pPos = ImGui::GetCursorScreenPos();
            ImGui::BeginGroup(); // Lock horizontal position
            ImGui::Text("%s", "test");
            // Inputs
            ImGui::BeginGroup();
            for (auto& in : node->getInputs())
            {
                DrawNodeInput(in);
            }

            ImGui::EndGroup();
            ImGui::SameLine();
            // Outputs
            ImGui::BeginGroup();
            ImGui::NewLine();
            for (auto& out : node->getOutputs())
            {
                out.mPos = ImGui::GetCursorScreenPos();
                ImGui::Text("%s", out.mText.c_str());
                out.mSize = ImGui::GetItemRectSize();
            }
            ImGui::EndGroup();
            ImGui::EndGroup();

            // Save the size of what we have emitted and whether any of the widgets are being used
            bool node_widgets_active = (!old_any_active && ImGui::IsAnyItemActive());
            ImVec2 s = ImGui::GetItemRectSize() + NODE_WINDOW_PADDING + NODE_WINDOW_PADDING;
            node->setSize(s);
            ImVec2 node_rect_max = node_rect_min + node->getSize();

            std::vector<ImVec2> nodeSlots; // contains positions of all node slots
            // LinkButtons
            // Input
            auto& inputs = node->getInputs();
            for (int slot_idx = 0; slot_idx < inputs.size(); slot_idx++)
            {
                if (inputs[slot_idx].mPluggable)
                {
                    ImVec2 pos = GetNodeInputPos(node.get(), slot_idx);
                    ImVec2 size = inputs[slot_idx].mSize;
                    ImVec2 nPos = {};
                    nPos.x = offset.x + pos.x;
                    nPos.y = pos.y + (size.y / 2.0f);
                    ImGui::PushID(slot_idx);
                    ImGui::SetCursorScreenPos(nPos - ImVec2{ size.y / 2.0f, size.y / 2.0f });
                    ImGui::InvisibleButton("##inputLink", { size.y, size.y });
                    if (ImGui::BeginDragDropSource())
                    {
                        char* str = "INPUT_LINK";
                        if (linkDragMode == -1)
                        {
                            if (auto src = inputs[slot_idx].mSource.lock())
                            {
                                for (size_t nIdx = 0ull; nIdx < nodes.size(); nIdx++)
                                {
                                    if (nodes[nIdx].get() == src.get())
                                    {
                                        linkDragMode = OUTPUT_START_TYPE;
                                        linkNodeIndex = static_cast<int>(nIdx);
                                        linkNodeLnkIndex = inputs[slot_idx].mSourceIndex;
                                    }
                                }
                            }
                            else
                            {
                                linkDragMode = INPUT_START_TYPE;
                                linkNodeIndex = static_cast<int>(idx);
                                linkNodeLnkIndex = slot_idx;
                            }
                            inputs[slot_idx].mSource.reset();
                        }
                        if (linkDragMode == OUTPUT_START_TYPE)
                        {
                            str = "OUTPUT_LINK";

                        }

                        ImGui::SetDragDropPayload(str, nullptr, 0);
                        ImGui::EndDragDropSource();
                    }
                    if (ImGui::BeginDragDropTarget() && linkDragMode == OUTPUT_START_TYPE)
                    {
                        if (ImGui::AcceptDragDropPayload("OUTPUT_LINK"))
                        {
                            inputs[slot_idx].mSource = nodes[linkNodeIndex];
                            inputs[slot_idx].mSourceIndex = linkNodeLnkIndex;
                        }
                        ImGui::EndDragDropTarget();
                    }
                    ImGui::PopID();
                    nodeSlots.push_back(nPos);
                }

                if (auto src = inputs[slot_idx].mSource.lock())
                {
                    ImVec2 pos = GetNodeInputPos(node.get(), slot_idx);
                    ImVec2 size = inputs[slot_idx].mSize;
                    ImVec2 nPos = {};
                    nPos.x = offset.x + pos.x;
                    nPos.y = pos.y + (size.y / 2.0f);

                    imext::ShaderLink lnk;
                    lnk.mEndPos = nPos;

                    //imext::Output& out = src->getOutputs()[inputs[slot_idx].mSourceIndex];
                    pos = GetNodeOutputPos(src.get(), inputs[slot_idx].mSourceIndex);

                    lnk.mStartPos = pos;
                    lnk.mStartPos.x += offset.x;
                    lnk.mStartPos.y += (size.y / 2.0f);
                    links.push_back(lnk);
                }
            }
            // Output
            auto& outputs = node->getOutputs();
            for (int slot_idx = 0; slot_idx < outputs.size(); slot_idx++)
            {
                ImVec2 pos = GetNodeOutputPos(node.get(), slot_idx);
                ImVec2 size = outputs[slot_idx].mSize;
                ImVec2 nPos = {};
                nPos.x = offset.x + pos.x;
                nPos.y = pos.y + (size.y / 2.0f);
                ImGui::PushID(slot_idx);
                ImGui::SetCursorScreenPos(nPos - ImVec2{ size.y / 2.0f, size.y / 2.0f });
                ImGui::InvisibleButton("##outputLink", { size.y, size.y });
                if (ImGui::BeginDragDropSource())
                {
                    linkDragMode = OUTPUT_START_TYPE;
                    linkNodeIndex = static_cast<int>(idx);
                    linkNodeLnkIndex = slot_idx;
                    ImGui::SetDragDropPayload("OUTPUT_LINK", nullptr, 0);
                    ImGui::EndDragDropSource();
                }
                if (ImGui::BeginDragDropTarget() && linkDragMode == INPUT_START_TYPE)
                {
                    if (ImGui::AcceptDragDropPayload("INPUT_LINK"))
                    {
                        auto& inputNode = nodes[linkNodeIndex];
                        auto& input = inputNode->getInputs()[linkNodeLnkIndex];
                        input.mSource = node;
                        input.mSourceIndex = slot_idx;
                    }
                    ImGui::EndDragDropTarget();
                }
                ImGui::PopID();
                nodeSlots.push_back(nPos);
            }

            // Display node box
            draw_list->ChannelsSetCurrent(static_cast<int>(idx)*2); // Background
            ImGui::SetCursorScreenPos(node_rect_min);
            ImGui::InvisibleButton("node", node->getSize());
            ImGui::SetItemAllowOverlap();
            if (ImGui::IsItemHovered() && !isHover)
            {
                node_hovered_in_scene = static_cast<int>(idx);
                isHover = true;
            }

            bool node_moving_active = ImGui::IsItemActive();
            if (node_widgets_active || node_moving_active)
            {
                selectedNode = static_cast<int>(idx);
                newActive = true;
            }
            if (node_moving_active && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
                nodes[idx]->setPos(nodes[idx]->getPos() + io.MouseDelta);

            ImU32 node_bg_color = (node_hovered_in_scene == idx || (selectedNode == idx)) ? IM_COL32(75, 75, 75, 255)
                                                                                      : IM_COL32(60, 60, 60, 255);
            draw_list->AddRectFilled(node_rect_min, node_rect_max, node_bg_color, 4.0f);
            draw_list->AddRect(node_rect_min, node_rect_max, IM_COL32(100, 100, 100, 255), 4.0f);

            // Render Node slots
            for (auto& pos : nodeSlots)
            {
                draw_list->AddCircleFilled(pos, NODE_SLOT_RADIUS, IM_COL32(150, 150, 150, 150));
            }
            ImGui::PopID();
        }

        if (!ImGui::IsDragDropActive())
        {
            linkDragMode = -1;
        }

        if (linkDragMode == INPUT_START_TYPE)
        {
            auto& node = nodes[linkNodeIndex];
            auto& input = node->getInputs()[linkNodeLnkIndex];
            ShaderLink lnk{};
            lnk.mStartPos = GetNodeInputPos(node.get(), linkNodeLnkIndex);
            lnk.mStartPos.x += offset.x;
            lnk.mStartPos.y += (input.mSize.y / 2.0f);

            lnk.mEndPos = ImGui::GetMousePos();
            links.push_back(lnk);
        }
        else if (linkDragMode == OUTPUT_START_TYPE)
        {
            auto& node = nodes[linkNodeIndex];
            auto& output = node->getOutputs()[linkNodeLnkIndex];
            ShaderLink lnk{};
            lnk.mStartPos = GetNodeOutputPos(node.get(), linkNodeLnkIndex);
            lnk.mStartPos.x += offset.x;
            lnk.mStartPos.y += (output.mSize.y / 2.0f);

            lnk.mEndPos = ImGui::GetMousePos();
            links.push_back(lnk);
        }
        else
        {
            // do nothing
        }

        if (newActive)
        {
            nodes[selectedNode].swap(nodes.back());
            selectedNode = static_cast<int>(nodes.size() - 1ull);
            //selectedNode = 0;
        }

        // render links
        draw_list->ChannelsSetCurrent(channelCount - 1ull); // Background
        for (auto& link : links)
        {
            ImVec2 startPos = link.mStartPos;
            ImVec2 endPos = link.mEndPos;
            draw_list->AddBezierCubic(startPos, startPos + ImVec2(+50, 0), endPos + ImVec2(-50, 0), endPos,
                                      IM_COL32(200, 200, 100, 255), 3.0f);
        }

        draw_list->ChannelsMerge();

        // Scrolling
        if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() &&
            ImGui::IsMouseDragging(ImGuiMouseButton_Middle, 0.0f))
        {
            scrolling = scrolling + io.MouseDelta;
            storage->SetFloat(SCROLL_X_LOC, scrolling.x);
            storage->SetFloat(SCROLL_Y_LOC, scrolling.y);
        }
        storage->SetInt(SELECTED_NODE, selectedNode);
        storage->SetInt(LINK_START_TYPE, linkDragMode);
        storage->SetInt(LINK_START_NODE_IDX, linkNodeIndex);
        storage->SetInt(LINK_START_LNK_IDX, linkNodeLnkIndex);

        ImGui::PopItemWidth();
        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
        ImGui::EndGroup();

        ImGui::End();
    }

#endif

} // namespace imext

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
