//
// Created by fcx on 2020/6/5.
//

#ifndef CLOUDLARKXR_ASSET_FILES_H
#define CLOUDLARKXR_ASSET_FILES_H


#include "asset_loader.h"

namespace {
const lark::AssetLists Assetlist = {
        // shader
        {
                { lark::ShaderAssetType_File, "object", "shader/vertex/color_vertex.glsl", "shader/fragment/color_fragment.glsl", "", ""},
                { lark::ShaderAssetType_File, "text", "shader/vertex/text_vertex.glsl", "shader/fragment/text_fragment.glsl", "", ""},
                { lark::ShaderAssetType_File, "text", "shader/vertex/image_vertex.glsl", "shader/fragment/image_fragment.glsl", "", ""},
        },
        // texture
        {
                // skybox
                { lark::TextureAssetType_Local_Skybox, "textures/skybox_8_2k.jpg" },
                { lark::TextureAssetType_Local_Normal, "textures/ui/arrow_back.png" },
                { lark::TextureAssetType_Local_Normal, "textures/ui/arrow_right.png" },
                { lark::TextureAssetType_Local_Normal, "textures/ui/bg.png" },
                { lark::TextureAssetType_Local_Normal, "textures/ui/border.png" },
                { lark::TextureAssetType_Local_Normal, "textures/ui/border_selected.png" },
                { lark::TextureAssetType_Local_Normal, "textures/ui/circle.png" },
                { lark::TextureAssetType_Local_Normal, "textures/ui/circle_active.png" },
                { lark::TextureAssetType_Local_Normal, "textures/ui/circle_huakuai.png" },
                { lark::TextureAssetType_Local_Normal, "textures/ui/cover_10.png" },
                { lark::TextureAssetType_Local_Normal, "textures/ui/data.png" },
                { lark::TextureAssetType_Local_Normal, "textures/ui/eye.png" },
                { lark::TextureAssetType_Local_Normal, "textures/ui/eye_new.png" },
                { lark::TextureAssetType_Local_Normal, "textures/ui/hight.png" },
                { lark::TextureAssetType_Local_Normal, "textures/ui/hight_new.png" },
                { lark::TextureAssetType_Local_Normal, "textures/ui/icon_set.png" },
                { lark::TextureAssetType_Local_Normal, "textures/ui/icon_set_hover.png" },
                { lark::TextureAssetType_Local_Normal, "textures/ui/icon_set_hover.png" },
                { lark::TextureAssetType_Local_Normal, "textures/ui/left_hand_active.png" },
                { lark::TextureAssetType_Local_Normal, "textures/ui/left_hand_nor.png" },
                { lark::TextureAssetType_Local_Normal, "textures/ui/line.png" },
                { lark::TextureAssetType_Local_Normal, "textures/ui/loading.png" },
                { lark::TextureAssetType_Local_Normal, "textures/ui/logo.png" },
                { lark::TextureAssetType_Local_Normal, "textures/ui/page_left.png" },
                { lark::TextureAssetType_Local_Normal, "textures/ui/page_left_hover.png" },
                { lark::TextureAssetType_Local_Normal, "textures/ui/page_right.png" },
                { lark::TextureAssetType_Local_Normal, "textures/ui/page_right_hover.png" },
                { lark::TextureAssetType_Local_Normal, "textures/ui/right_hand_active.png" },
                { lark::TextureAssetType_Local_Normal, "textures/ui/right_hand_nor.png" },
                { lark::TextureAssetType_Local_Normal, "textures/ui/safe.png" },
                { lark::TextureAssetType_Local_Normal, "textures/ui/safe_new.png" },
                { lark::TextureAssetType_Local_Normal, "textures/ui/web_client_loading_img.png" },
        },
        // model
        {}
};
}
#endif //CLOUDLARKXR_ASSET_FILES_H
