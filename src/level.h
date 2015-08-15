// Written by Michael Zeilfelder, please check licenseHCraft.txt for the zlib-style license text.

#ifndef LEVEL_H
#define LEVEL_H

#include "globals.h"
#include "ai_track.h"
#include <string>
#include <vector>

class TiXmlElement;
struct LevelSettings;

struct TrackStartSettings
{
    TrackStartSettings() : mIsValid(false) {}
    void ReadFromXml(const TiXmlElement * settings_);
    void WriteToXml(TiXmlElement * settings_) const;
    void Reset()    { mIsValid = false; }

    bool            mIsValid;
    irr::core::vector3df mCenter;
    irr::core::vector3df mRotation;
};

struct TrackMarkerSettings
{
    TrackMarkerSettings();
    void ReadFromXml(const TiXmlElement * settings_);
    void WriteToXml(TiXmlElement * settings_) const;
    void Reset();
    void EnableWalls();

    bool            mIsValid;
    irr::core::vector3df mCenter;
    irr::core::vector3df mUpVector;
    irr::core::vector3df mRotation;
    bool            mRelocate;  // hover can teleport to this track when he drops off
    bool            mUseDefaultSizes;
    float           mLeft;
    float           mRight;
    float           mTop;
    float           mBottom;

    bool            mHasLeftWall;
    bool            mHasRightWall;
    bool            mHasBottomWall;
//    bool            mHasTopWall;
};

struct TrackStart
{
    TrackStart() : mEditNode(0) {}

    bool GetCenter( irr::core::vector3df &pos_);

    irr::scene::ISceneNode*  mEditNode;
    TrackStartSettings  mSettings;
};

struct TrackMarker
{
    TrackMarker();

    bool GetCenter( irr::core::vector3df &pos_);

    irr::scene::ISceneNode*  mEditNodeCenter;
    irr::scene::ISceneNode*  mEditNodeLeftTop;
    irr::scene::ISceneNode*  mEditNodeRightBottom;
    irr::scene::ISceneNode*  mNodeCollision;
    irr::scene::ISceneNode*  mNodeWallLeft;
    irr::scene::ISceneNode*  mNodeWallRight;
    irr::scene::ISceneNode*  mNodeWallTop;
    irr::scene::ISceneNode*  mNodeWallBottom;

    TrackMarkerSettings mSettings;
};


struct LevelModelSettings
{
    LevelModelSettings();
    void ReadFromXml(const TiXmlElement * settings_);
    void WriteToXml(TiXmlElement * settings_) const;

    std::string     mName;
    irr::core::vector3df mCenter;
    irr::core::vector3df mRotation;
    irr::core::vector3df mScale;
    bool            mHasCollisions;

    // those ambient values will influence all meshes which DO NOT have ambience already set to 0
    int             mAmbientRed;
    int             mAmbientGreen;
    int             mAmbientBlue;
};

struct LevelModel
{
    LevelModel();

    bool GetCenter( irr::core::vector3df &pos_);

    irr::scene::ISceneNode*  mNodeModel;
    LevelModelSettings  mSettings;
};

class Level
{
public:
    Level();
    ~Level();

	bool Load(LevelSettings * settings_);
    bool SaveTrackData();

    irr::scene::ISceneNode* GetNodeCamera() { return mNodeCamera; }

    void ShowEditData(bool enable_);

    bool CheckWallCollision(const irr::core::line3d<irr::f32> &moveLine_, int &index_) const;
    bool CheckMarkerCollision(const irr::core::line3d<irr::f32> &moveLine_, int &index_) const;
    bool CheckFinishLineCollision(const irr::core::line3d<irr::f32> &moveLine_) const;
    bool CheckTeleportLineCollision(const irr::core::line3d<irr::f32> &moveLine_, irr::core::vector3df &targetPos_, irr::core::vector3df &rotation_, irr::core::vector3df &velocity_) const;

    int GetTrackRelocatesBetween(int indexFront_, int indexBack_);

    // remember: GetTrackMarker and GetModel return no references because they are part of a vector which can change
    TrackStart& GetTrackStart(unsigned int index_);
    unsigned int GetNrOfTrackMarkers() const;
    TrackMarker GetTrackMarker(unsigned int index_) const { return mTrackMarkers[index_]; }
    TrackMarker& GetFinishLine()                   { return mFinishLine; }
    TrackMarker& GetTpSource()                     { return mTpSource; }
    TrackMarker& GetTpTarget()                     { return mTpTarget; }
    LevelModel GetModel(unsigned int index_) const { return mModels[index_]; }
    unsigned int GetNrOfModels() const             { return mModels.size(); }
    AiTrack& GetAiTrack()                          { return mAiTrack; }

    void SetTrackStart(const TrackStartSettings& ts_, int player_);
    void SetTrackMarker(const TrackMarkerSettings &marker_, int index_);
    void InsertTrackMarker(const TrackMarkerSettings &marker_, int index_);
    void AppendTrackMarker(const TrackMarkerSettings &marker_, int index_);
    void RotateTrackMarker(float angle_, int index_);
    void AutoAlignTrackMarker(int index_);
    void ChangeTrackMarkerSizes(bool useDefault_, float changeLeft_, float changeRight_, float changeTop_, float changeBottom_, int index_);
    void RemoveTrackMarker(int index_);

    void SetTeleportSource(const TrackMarkerSettings &marker_);
    void SetTeleportTarget(const TrackMarkerSettings &marker_);

    void SetFinishLine(const TrackMarkerSettings &marker_);

    void AddModel(const std::string &name_);
    void SetModel(const LevelModelSettings &settings_, int index_);
    void ChangeModelRotation(float addRotX_, float addRotY_, float addRotZ_, int index_);
    void ChangeModelPosition(float addPosX_, float addPosY_, float addPosZ_, int index_);
    void SetModelAmbient(int ambR_, int ambG_, int ambB_, int index_);
    void RemoveModel(int index_);

    bool CheckTrackStartEditCollision(const irr::core::line3d<irr::f32> line_, int &index_, float &dist_);
    bool CheckTrackMarkerEditCollision(const irr::core::line3d<irr::f32> line_, int &index_, float &dist_);
    bool CheckTpSourceEditCollision(const irr::core::line3d<irr::f32> line_, float &dist_);
    bool CheckTpTargetEditCollision(const irr::core::line3d<irr::f32> line_, float &dist_);
    bool CheckFinishLineEditCollision(const irr::core::line3d<irr::f32> line_, int &index_, float &dist_);
    bool CheckModelEditCollision(const irr::core::line3d<irr::f32> line_, int &index_, float &dist_);

protected:
    bool CheckEditCollision(irr::scene::ISceneNode*, const irr::core::line3d<irr::f32> line_, float &dist_) const;
    // only two triangles checked!
    bool CheckLineNodeCollision2T(const irr::core::line3d<irr::f32> &line_, irr::scene::ISceneNode* node_, irr::core::vector3df &outIntersection_) const;

    void BuildTrackData(LevelSettings * settings_);
    void ClearTrackData();
    void ClearTrackMarkerData(TrackMarker & marker_);
    void ClearModelData(LevelModel & model_);
    void SetTrackMarkerSettings(const TrackMarkerSettings &settings_, TrackMarker & marker_);
    void SetCollisionWalls(TrackMarker & marker1_, const TrackMarker & marker2_ );
    void RemoveCollisionWalls(TrackMarker & marker_);
    void CalcMarkerBorders(const TrackMarker & marker_, irr::core::vector3df & leftTop_, irr::core::vector3df & rightTop_, irr::core::vector3df & leftBottom_, irr::core::vector3df & rightBottom_, bool relative_=true);
    irr::scene::ISceneNode* AddQuadradicNode(irr::scene::ISceneNode* parent_, const irr::core::vector3df &leftTop_, const irr::core::vector3df &rightTop_, const irr::core::vector3df &leftBottom_, const irr::core::vector3df &rightBottom_);

    bool LoadTrackData(const std::string &fileName_);

private:
    std::string mLevelFileName;
    irr::scene::ISceneNode* mLevelNode;
    irr::scene::ITriangleSelector* mSelector;

    irr::scene::ISceneNode* mNodeCamera;     // that node is always added to the active camera
    irr::scene::ISceneNode* mEditDataNode;   // parent for all edit nodes
    irr::scene::ISceneNode* mTrackDataNode;  // parent for all track data nodes

    std::string					mTrackDataFilename;
    TrackStart                  mTrackStarts[MAX_SPAWNS];
    std::vector<TrackMarker>    mTrackMarkers;
    TrackMarker                 mFinishLine;
    TrackMarker                 mTpSource;
    TrackMarker                 mTpTarget;
    std::vector<LevelModel>     mModels;
    AiTrack                     mAiTrack;
};

#endif // LEVEL_H
