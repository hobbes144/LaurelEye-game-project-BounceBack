#include "TestDefinitions.h"
#include "LaurelEyeEngine/io/importers/ImageImporter.h"
#include "LaurelEyeEngine/io/importers/MeshImporter.h"

namespace LaurelEye {
    void assetMeshTest() {
        IO::AssetManager assetManager;
        assetManager.registerImporter("obj", std::make_unique<IO::MeshImporter>());
        assetManager.registerImporter("fbx", std::make_unique<IO::MeshImporter>());

        try {
            auto asset = assetManager.load(std::string(TEST_MEDIA_DIR) + "/item-box.fbx");
            auto mesh = std::dynamic_pointer_cast<IO::MeshAsset>(asset);
            assert(mesh && "Mesh import failed: not a MeshAsset");
            assert(!mesh->vertices.empty() && "Mesh import failed: no vertices loaded");
            assert(!mesh->indices.empty() && "Mesh import failed: no indices loaded");

            std::cout << "[PASS] Mesh asset imported successfully ("
                      << mesh->vertices.size() << " verts, "
                      << mesh->indices.size() << " indices)\n";
        }
        catch ( const std::exception& e ) {
            std::cerr << "[FAIL] Mesh asset test: " << e.what() << "\n";
            assert(false);
        }
    }

    void assetImageTest() {
        IO::AssetManager assetManager;
        assetManager.registerImporter("png", std::make_unique<IO::ImageImporter>());
        assetManager.registerImporter("jpg", std::make_unique<IO::ImageImporter>());

        try {
           
            auto asset = assetManager.load(std::string(TEST_MEDIA_DIR) + "/variation-a.png");
            auto image = std::dynamic_pointer_cast<IO::ImageAsset>(asset);
            assert(image && "Image import failed: not an ImageAsset");
            assert(image->width > 0 && image->height > 0 && "Image import failed: invalid dimensions");
            assert(!image->pixelData.empty() && "Image import failed: no pixel data");

            std::cout << "[PASS] Image asset imported successfully ("
                      << image->width << "x" << image->height << ")\n";
        }
        catch ( const std::exception& e ) {
            std::cerr << "[FAIL] Image asset test: " << e.what() << "\n";
            assert(false);
        }
    }

    void assetCacheTest() {
        IO::AssetManager assetManager;
        assetManager.registerImporter("png", std::make_unique<IO::ImageImporter>());

        try {
            auto firstLoad = assetManager.load(std::string(TEST_MEDIA_DIR) + "/variation-a.png");
            auto secondLoad = assetManager.load(std::string(TEST_MEDIA_DIR) + "/variation-a.png");

            assert(firstLoad == secondLoad && "Cache test failed: load did not return cached asset");

            std::cout << "[PASS] Asset cache works correctly (same shared_ptr returned)\n";
        }
        catch ( const std::exception& e ) {
            std::cerr << "[FAIL] Asset cache test: " << e.what() << "\n";
            assert(false);
        }
    }
}
