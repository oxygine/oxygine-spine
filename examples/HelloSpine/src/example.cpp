#include "oxygine-framework.h"
#include <functional>
#include "oxygine-spine.h"

using namespace oxygine;


void example_preinit() {}


spine::AnimationStateData* animationStateData = 0;
spine::AnimationState* animationState = 0;
spine::SkeletonData *skeletonData = 0;
spine::Atlas* atlas = 0;
spine::Skeleton *skeleton = 0;
oxspine::spSpineActor actor;

//called from main.cpp
void example_init()
{
    key::init();

    
    oxspine::init();


    file::buffer bf;

    atlas = new spine::Atlas("spine/spineboy/spineboy.atlas", oxspine::getTextureLoader());

    spine::SkeletonBinary binary(atlas);
    binary.setScale(1);

    
    skeletonData = binary.readSkeletonDataFile("spine/spineboy/spineboy-ess.skel");


    animationStateData = new spine::AnimationStateData(skeletonData);

    animationState = new spine::AnimationState(animationStateData);
    animationState->setAnimation(0, "idle", true);
    

    skeleton = new spine::Skeleton(skeletonData);
    skeleton->setScaleY(-1);


    actor = new oxspine::SpineActor();
    actor->setY(getStage()->getHeight() - 100);
    actor->setX(getStage()->getWidth() / 2);
    actor->setScale(0.5f);
    actor->setSkeleton(skeleton);
    actor->setAnimationState(animationState);

    //and add it to Stage as child
    getStage()->addChild(actor);
}


//called each frame from main.cpp
void example_update()
{
    
    if (key::wasPressed(SDL_SCANCODE_RETURN))
    {
        animationState->setAnimation(1, "walk", true);
    }

    if (key::wasReleased(SDL_SCANCODE_RETURN))
    {
        animationState->setEmptyAnimation(1, 0.15f);
    }



    if (key::wasPressed(SDL_SCANCODE_SPACE))
    {
        animationState->setAnimation(2, "shoot", true);
    }

    if (key::wasReleased(SDL_SCANCODE_SPACE))
    {
        animationState->setEmptyAnimation(2, 0.15f);
    }

}

//called each frame from main.cpp
void example_destroy()
{
    actor = 0;

    delete skeleton;
    skeleton = 0;

    delete skeletonData;
    skeletonData = 0;

    delete animationState;
    animationState = 0;

    delete animationState;
    animationState = 0;

    delete animationStateData;
    animationStateData = 0;

    delete atlas;
    atlas = 0;

    
    key::release();
    oxspine::free();
}
