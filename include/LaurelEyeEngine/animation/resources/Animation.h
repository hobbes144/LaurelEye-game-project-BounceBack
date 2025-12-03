// Notes:
//
// This resource stores the animations from the animation file. This will store
// all the keyframe transforms from the channels in the bone index order from
// the skeleton.
//
// std::string name;
// struct Keyframe {
//     Transform localPose;
//     float timeStamp;
// };
// struct AnimationChannel {
//     std::vector<Keyframe> keyframes;
// };
// std::vector<AnimationChannel> channels;
//
// This is used by the AnimationComponent to calculate the current localPose
// transform for each frame at the animation time unit.
