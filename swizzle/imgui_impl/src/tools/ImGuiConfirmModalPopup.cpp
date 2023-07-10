
/* Include files */

#include <ImGuiSwizzleTools.hpp>

/* Defines */

/* Typedefs */

/* Structs/Classes */

/* Static Variables */

/* Static Function Declaration */

/* Static Function Definition */

/* Function Definition */

namespace imext
{
    int ConfirmModalPopup(const char* label, ImGuiWindowFlags_ flags, const std::string& description,
                          const std::vector<std::string>& options)
    {
        int ret = -1;
        if (ImGui::BeginPopupModal(label, 0, flags))
        {
            ImGui::Text(description.c_str());

            int ctr = 0;
            for (const auto& opt : options)
            {
                if (ctr != 0)
                {
                    ImGui::SameLine();
                }
                if (ImGui::Button(opt.c_str()))
                {
                    ret = ctr;
                    ImGui::CloseCurrentPopup();
                }
                ctr++;
            }

            ImGui::EndPopup();
        }

        return ret;
    }

} // namespace imext

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
