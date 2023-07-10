
/* Include files */

#include <ImGuiSwizzleTools.hpp>

/* Defines */

/* Typedefs */

/* Structs/Classes */

namespace imext
{
    struct TextInputData
    {
        std::string* mStr;
    };
} // namespace imext

/* Static Variables */

/* Static Function Declaration */

namespace imext
{
    static int InputTextCallback(ImGuiInputTextCallbackData* data);
}

/* Static Function Definition */

namespace imext
{
    static int InputTextCallback(ImGuiInputTextCallbackData* data)
    {
        TextInputData* userData = (TextInputData*)data->UserData;
        if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
        {
            // Resize string callback
            // If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we need to set them
            // back to what we want.
            std::string* str = userData->mStr;
            IM_ASSERT(data->Buf == str->c_str());
            str->resize(data->BufTextLen);
            data->Buf = (char*)str->c_str();
        }
        return 0;
    }
} // namespace imext

/* Function Definition */

namespace imext
{
    bool InputText(const char* label, std::string& str)
    {
        TextInputData cb_user_data;
        cb_user_data.mStr = &str;
        return ImGui::InputText(label, str.data(), str.capacity() + 1,
                                ImGuiInputTextFlags_CallbackResize | ImGuiInputTextFlags_EnterReturnsTrue,
                                InputTextCallback, &cb_user_data);
    }

} // namespace imext

/* Class Public Function Definition */

/* Class Protected Function Definition */

/* Class Private Function Definition */
