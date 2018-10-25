#include "spine.h"
#include <spine/spine.h>
#include <spine/BlendMode.h>
#include <spine/Atlas.h>
#include <spine/RegionAttachment.h>
#include <spine/Debug.h>

#include "ox/STDRenderer.hpp"
#include "ox/file.hpp"
#include "ox/Image.hpp"
#include "ox/RenderState.hpp"

namespace spine
{
    static DebugExtension *debugExtension = NULL;

    class OxSpineExt : public DefaultSpineExtension
    {
    public:
        char *_readFile(const String &path, int *length) override
        {
            ox::file::handle h = ox::file::open(path.buffer(), "rb");
            if (!h)
                return 0;

            unsigned int size = ox::file::size(h);
            char *mem = (char*)_alloc(size, __FILE__, __LINE__);
            ox::file::read(h, mem, size);
            ox::file::close(h);

            *length = size;
            return mem;
        }
    };


    SpineExtension* getDefaultExtension() {
        // return a default spine extension that uses standard malloc for memory
        // management, and wrap it in a debug extension.

        static OxSpineExt ext;
        return &ext;

        //default DebugExtension is NOT THREAD SAFE, not recommended!!!
        debugExtension = new DebugExtension(new OxSpineExt());
        return debugExtension;
    }
}



namespace oxspine
{
    using namespace spine;


    void init()
    {

    }

    void free()
    {
#ifdef OX_DEBUG
        if (debugExtension)
        {
            debugExtension->reportLeaks();
        }
#endif
    }


    

    class MyTextureLoader : public TextureLoader 
    {
    public:
        MyTextureLoader() {}
        

        // Called when the atlas loads the texture of a page.
        virtual void load(AtlasPage& page, const String& path)
        {
            spNativeTexture texture = IVideoDriver::instance->createTexture();
            texture->addRef();
            //Texture* texture = engine_loadTexture(path);


            file::buffer buf;
            file::read(path.buffer(), buf);

            Image im;
            im.init(buf, true);


            texture->init(im.lock());

            // store the Texture on the rendererObject so we can
            // retrieve it later for rendering.
            page.setRendererObject(texture.get());

            // store the texture width and height on the spAtlasPage
            // so spine-c can calculate texture coordinates for
            // rendering.
            page.width = texture->getWidth();
            page.height = texture->getHeight();
        }

        // Called when the atlas is disposed and itself disposes its atlas pages.
        virtual void unload(void* texture) 
        {
            ((NativeTexture*)texture)->releaseRef();
        }
    };

    MyTextureLoader _loader;


    spine::TextureLoader * getTextureLoader()
    {
        return &_loader;
    }

    SpineActor::SpineActor()
    {

    }

    SpineActor::~SpineActor()
    {

    }

    void SpineActor::setSkeleton(spine::Skeleton* sk)
    {
        _skeleton = sk;
    }


    void SpineActor::setAnimationState(spine::AnimationState *st)
    {
        _animationState = st;
    }

    void SpineActor::doUpdate(const UpdateState& us)
    {
        _animationState->update(us.dt / 1000.0f);
        _animationState->apply(*_skeleton);
        _skeleton->updateWorldTransform();
    }

    void SpineActor::doRender(const RenderState& rs)
    {            
        Material::null->apply();

        STDRenderer *renderer = STDRenderer::getCurrent();
        renderer->setUberShaderProgram(&STDRenderer::uberShader);
        renderer->setShaderFlags(0);
        //renderer->setTransform(rs.transform);
        //renderer->setViewProj(renderer->getViewProjection());
        
        int alpha = rs.alpha;        

        blend_mode currentBlendMode = blend_disabled;
        NativeTexture *currentTexture = 0;

        rsCache().reset();

        for (size_t i = 0, n = _skeleton->getSlots().size(); i < n; ++i) 
        {
            Slot* slot = _skeleton->getDrawOrder()[i];

            // Fetch the currently active attachment, continue
            // with the next slot in the draw order if no
            // attachment is active on the slot
            Attachment* attachment = slot->getAttachment();
            if (!attachment) 
                continue;

            // Fetch the blend mode from the slot and
            // translate it to the engine blend mode
                 
                
            blend_mode engineBlendMode;

            switch (slot->getData().getBlendMode()) 
            {
            case BlendMode_Normal:
                engineBlendMode = blend_premultiplied_alpha;
                break;
            case BlendMode_Additive:
                engineBlendMode = blend_add;
                break;
            case BlendMode_Multiply:
                engineBlendMode = blend_multiply;
                break;
            case BlendMode_Screen:
                engineBlendMode = blend_screen;
                break;
            default:
                // unknown Spine blend mode, fall back to
                // normal blend mode
                engineBlendMode = blend_premultiplied_alpha;
            }

            if (currentBlendMode != engineBlendMode)
            {
                renderer->flush();
                currentBlendMode = engineBlendMode;
                rsCache().setBlendMode(engineBlendMode);
            }

            // Calculate the tinting color based on the skeleton's color
            // and the slot's color. Each color channel is given in the
            // range [0-1], you may have to multiply by 255 and cast to
            // and int if your engine uses integer ranges for color channels.
            spine::Color skeletonColor = _skeleton->getColor();
            spine::Color slotColor = slot->getColor();
            spine::Color tint(skeletonColor.r * slotColor.r, skeletonColor.g * slotColor.g, skeletonColor.b * slotColor.b, skeletonColor.a * slotColor.a);

            oxygine::Color color = oxygine::Color(tint.r * 255, tint.g * 255, tint.b * 255, tint.a * 255);
            color.a = (color.a * alpha) / 255;
            color = color.premultiplied();

            // Fill the vertices array, indices, and texture depending on the type of attachment
            if (attachment->getRTTI().isExactly(RegionAttachment::rtti))
            {
                // Cast to an spRegionAttachment so we can get the rendererObject
                // and compute the world vertices
                RegionAttachment* regionAttachment = (RegionAttachment*)attachment;

                // Our engine specific Texture is stored in the AtlasRegion which was
                // assigned to the attachment on load. It represents the texture atlas
                // page that contains the image the region attachment is mapped to.
                NativeTexture* texture = (NativeTexture*)((AtlasRegion*)regionAttachment->getRendererObject())->page->getRendererObject();
                if (currentTexture != texture)
                {
                    renderer->flush();
                    currentTexture = texture;
                    rsCache().setTexture(0, texture);
                }
                

                Vector2 positions[4];                
                regionAttachment->computeWorldVertices(slot->getBone(), (float*)positions, 0, sizeof(Vector2)/sizeof(float));

                // copy color and UVs to the vertices

                vertexPCT2 v[4];                
                for (size_t j = 0, l = 0; j < 4; j++, l += 2) 
                {
                    vertexPCT2& vertex = v[j];
                    Vector2 p = rs.transform.transform(positions[j]);
                    vertex.x = p.x;
                    vertex.y = p.y;
                    vertex.z = 0;
                    vertex.color = color.rgba();
                    vertex.u = regionAttachment->getUVs()[l];
                    vertex.v = regionAttachment->getUVs()[l + 1];
                }

                vertexPCT2 a = v[3];
                v[3] = v[2];
                v[2] = a;

                renderer->addVertices(v, sizeof(v));
            }
            else if (attachment->getRTTI().isExactly(MeshAttachment::rtti)) {
                // Cast to an spMeshAttachment so we can get the rendererObject
                // and compute the world vertices
                logs::messageln("mesh render not implemented");
                /*
                MeshAttachment* mesh = (MeshAttachment*)attachment;

                // Ensure there is enough room for vertices
                vertices.setSize(mesh->getWorldVerticesLength() / 2, Vertex());

                // Our engine specific Texture is stored in the AtlasRegion which was
                // assigned to the attachment on load. It represents the texture atlas
                // page that contains the image the region attachment is mapped to.
                texture = (Texture*)((AtlasRegion*)regionAttachment->getRendererObject())->page->getRendererObject();

                // Computed the world vertices positions for the vertices that make up
                // the mesh attachment. This assumes the world transform of the
                // bone to which the slot (and hence attachment) is attached has been calculated
                // before rendering via Skeleton::updateWorldTransform(). The vertex positions will
                // be written directly into the vertices array, with a stride of sizeof(Vertex)
                size_t numVertices = mesh->getWorldVerticesLength() / 2;
                mesh->computeWorldVertices(slot, 0, numVertices, vertices.buffer(), 0, sizeof(Vertex));

                // Copy color and UVs to the vertices
                for (size_t j = 0, l = 0; j < numVertices; j++, l += 2) {
                    Vertex&amp; vertex = vertices[j];
                    vertex.color.set(tint);
                    vertex.u = regionAttachment->getUVs[l];
                    vertex.v = regionAttachment->getUVs[l + 1];
                }

                // set the indices, 2 triangles forming a quad
                indices = quadIndices;
                */
            }       
        }

        renderer->flush();
    }
}
