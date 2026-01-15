// /*!****************************************************************************
//  * \file   RenderGraphBuilder.h
//  * \author Anish Murthy (anish.murthy.dev@gmail.com)
//  * \par    **DigiPen Email**
//  *    anish.murthy@digipen.edu
//  * \date   04-09-2025
//  * \brief  Render Graph Builder that manager resources related to the render
//  * graph
//  *
//  * Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.
//  *
//  *****************************************************************************/
// #pragma once
//
// // #include "RenderFlags.h"
// // #include "TextureManager.h"
// #include "FBO.h"
//
// class Renderer;
// class ShadowManager;
//
// class RenderGraphBuilder {
// public:
//   RenderGraphBuilder(Renderer* _renderer) : renderer(_renderer) {}
//   ~RenderGraphBuilder() { deleteResources(); };
//
//   FBO* createScreenSizeFBO
//   (const std::string& name, std::vector<std::string> attachments);
//   FBO* createFBO(
//     const std::string& name, std::vector<std::string> attachments, int width, int height);
//   // TextureManager::TextureID createScreenSizeTexture(
//   //   const std::string& name);
//   // TextureManager::TextureID createTexture(
//   //   const std::string& name, int width, int height);
//
//   FBO* getFBO(const std::string& name);
//   // TextureManager::TextureID getTexture(const std::string& name);
//
//   void clearFBOs();
//
//   void deleteFBOs();
//   // void deleteTextures();
//   void deleteResources();
//
//   Renderer* getRenderer() { return renderer; };
//   void setShadowManager(ShadowManager* _shadowManager) { shadowManager = _shadowManager; };
//   ShadowManager* getShadowManager() { return shadowManager; };
//
// private:
//   Renderer* renderer;
//   ShadowManager* shadowManager;
//   // std::unordered_map<std::string, TextureManager::TextureID> textures;
//   std::unordered_map<std::string, FBO*> fbos;
// };
