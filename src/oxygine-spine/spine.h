#pragma once
#include <spine/spine.h>
#include "ox/Actor.hpp"


namespace oxspine
{
    using namespace oxygine;

    void init();
    void free();

    spine::TextureLoader *getTextureLoader();

    DECLARE_SMART(SpineActor, spSpineActor);
    class SpineActor : public Actor
    {
    public:
        SpineActor();
        ~SpineActor();

        void setSkeleton(spine::Skeleton*);
        void setAnimationState(spine::AnimationState *st);


    protected:
        void doUpdate(const UpdateState& us);
        void doRender(const RenderState& rs);

        spine::Skeleton *_skeleton;
        spine::AnimationState *_animationState;
    };
}
