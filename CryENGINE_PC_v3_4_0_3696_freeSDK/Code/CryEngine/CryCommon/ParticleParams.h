////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   ParticleParams.h
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef _PARTICLEPARAMS_H_
#define _PARTICLEPARAMS_H_ 1

#include <FinalizingSpline.h>
#include <Cry_Color.h>
#include <CryString.h>
#include <CryName.h>

#include <CryCustomTypes.h>
#include <Cry_Math.h>

BASIC_TYPE_INFO(CCryName)

enum EParticleBlendType
{
  ParticleBlendType_AlphaBased			= OS_ALPHA_BLEND,
  ParticleBlendType_Additive				= OS_ADD_BLEND,
	ParticleBlendType_Multiplicative	= OS_MULTIPLY_BLEND,
  ParticleBlendType_Opaque					= 0,
};

enum EParticleFacing
{
	ParticleFacing_Camera,
	ParticleFacing_Free,
	ParticleFacing_Velocity,
	ParticleFacing_Horizontal,
	ParticleFacing_Water,
	ParticleFacing_Terrain,
	ParticleFacing_Decal,
};

enum EParticlePhysicsType
{
	ParticlePhysics_None,
	ParticlePhysics_SimpleCollision,
	ParticlePhysics_SimplePhysics,
	ParticlePhysics_RigidBody
};

enum EParticleForceType
{
	ParticleForce_None,
	ParticleForce_Wind,
	ParticleForce_Gravity,
	ParticleForce_Target,
};

enum EParticleHalfResFlag
{
	ParticleHalfResFlag_NotAllowed,
	ParticleHalfResFlag_Allowed,
	ParticleHalfResFlag_Forced,
};

enum ETrinary
{
	Trinary_Both,
	Trinary_If_True,
	Trinary_If_False
};

inline bool TrinaryMatch(ETrinary t1, ETrinary t2)
{
	return (t1 | t2) != 3;
}

inline bool TrinaryMatch(ETrinary t, bool b)
{
	return t != (Trinary_If_True + (int)b);
}

enum EConfigSpecBrief
{
	ConfigSpec_Low = 0,
	ConfigSpec_Medium,
	ConfigSpec_High,
	ConfigSpec_VeryHigh,
};

// Sound related
enum ESoundControlTime
{
	SoundControlTime_EmitterLifeTime,
	SoundControlTime_EmitterExtendedLifeTime,
	SoundControlTime_EmitterPulsePeriod
};

// Pseudo-random number generation, from a key.
class CChaosKey
{
public:
	// Initialize with an int.
	explicit inline CChaosKey(uint32 uSeed)
		: m_Key(uSeed) {}

	explicit inline CChaosKey(float fSeed)
		: m_Key((uint32)(fSeed * float(0xFFFFFFFF))) {}

	CChaosKey Jumble(CChaosKey key2) const
	{
		return CChaosKey( Jumble(m_Key ^ key2.m_Key) );
	}
	CChaosKey Jumble(void const* ptr) const
	{
		return CChaosKey( Jumble(m_Key ^ (uint32)((UINT_PTR)ptr & 0xFFFFFFFF)) );
	}

	// Scale input range.
	inline float operator *(float fRange) const
	{
		return (float)m_Key / float(0xFFFFFFFF) * fRange;
	}
	inline uint32 operator *(uint32 nRange) const
	{
		return m_Key % nRange;
	}

	uint32 GetKey() const
	{
		return m_Key;
	}

	// Jumble with a range variable to produce a random value.
	template<class T>
	inline T Random(T const* pRange) const
	{
		return Jumble(CChaosKey(uint32(pRange))) * *pRange;
	}

private:
	uint32	m_Key;

	static inline uint32 Jumble(uint32 key)
	{
		key += ~rot_left(key, 15);
		key ^= rot_right(key, 10);
		key += rot_left(key, 3);
		key ^= rot_right(key, 6);
		key += ~rot_left(key, 11);
		key ^= rot_right(key, 16);
		return key;
	}

	static inline uint32 rot_left(uint32 u, int n)
	{
		return (u<<n) | (u>>(32-n));
	}
	static inline uint32 rot_right(uint32 u, int n)
	{
		return (u>>n) | (u<<(32-n));
	}
};

//////////////////////////////////////////////////////////////////////////
//
// Float storage
//

#if !defined(PARTICLE_COMPRESSED_FLOATS)

	typedef TRangedType<float>					SFloat;
	typedef TRangedType<float,0>				UFloat;
	typedef TRangedType<float,0,1>			UnitFloat;
	typedef TRangedType<float,0,2>			Unit2Float;
	typedef TRangedType<float,-180,180>	SAngle;
	typedef TRangedType<float,0,180>		UHalfAngle;
	typedef TRangedType<float,0,360>		UFullAngle;

#else

	typedef SFloat16										SFloat;
	typedef UFloat16										UFloat;
	typedef UnitFloat8									UnitFloat;
	typedef TFixed<uint8,2>							Unit2Float8;
	typedef TFixed<int16,180>						SAngle;
	typedef TFixed<uint16,180>					UHalfAngle;
	typedef TFixed<uint16,360>					UFullAngle;

#endif

typedef TSmall<bool> TSmallBool;

template<class T>
struct TStorageTraits
{
	typedef float				TValue;
	typedef UnitFloat		TRandom;
};

template<>
struct TStorageTraits<SFloat>
{
	typedef float				TValue;
	typedef Unit2Float	TRandom;
};

template<>
struct TStorageTraits<SAngle>
{
	typedef float				TValue;
	typedef Unit2Float	TRandom;
};

//////////////////////////////////////////////////////////////////////////
//
// Vec3 TypeInfo
//

// Must override Vec3 constructor to avoid polluting params with NANs
template<class T>
struct Vec3_Zero: Vec3_tpl<T>
{
	Vec3_Zero() : Vec3_tpl<T>(ZERO) {}
	Vec3_Zero(const Vec3& v) : Vec3_tpl<T>(v) {}
};

template<class T>
Vec3 BiRandom( const Vec3_Zero<T>& v )
{
	return BiRandom( Vec3(v) );
}

typedef Vec3_Zero<SFloat> Vec3S;
typedef Vec3_Zero<UFloat> Vec3U;

//////////////////////////////////////////////////////////////////////////
//
// Color specialisations.

template<class T>
struct Color3: Vec3_tpl<T>
{
	Color3( T v = T(0) )
		: Vec3_tpl<T>(v) {}

	Color3( T r, T g, T b )
		: Vec3_tpl<T>(r, g, b) {}

	template<class T2>
	Color3( Vec3_tpl<T2> const& c )
		: Vec3_tpl<T>(c) {}

	operator ColorF() const
		{ return ColorF(this->x, this->y, this->z, 1.f); }

	// Implement color multiplication.
	Color3& operator *=(Color3 const& c)
		{ this->x *= c.x; this->y *= c.y; this->z *= c.z; return *this; }
};

template<class T>
Color3<T> operator *(Color3<T> const& c, Color3<T> const& d)
	{ return Color3<T>(c.x * d.x, c.y * d.y, c.z * d.z); }

typedef Color3<float> Color3F;
typedef Color3< TFixed<uint8,1> > Color3B;

class RandomColor
{
public:
	inline RandomColor(float f = 0.f, bool bRandomHue = false)
		: m_VarRandom(f), m_bRandomHue(bRandomHue)
	{}
	inline operator bool() const
		{ return !!m_VarRandom; }
	inline bool operator!() const
		{ return !m_VarRandom; }

	Color3F GetRandom() const
	{
		if (m_bRandomHue)
		{
			ColorB clr(cry_rand32());
			float fScale = m_VarRandom / 255.f;
			return Color3F(clr.r * fScale, clr.g * fScale, clr.b * fScale);
		}
		else
		{
			return Color3F(Random(m_VarRandom));
		}
	}

	AUTO_STRUCT_INFO

protected:
	UnitFloat8	m_VarRandom;
	TSmallBool	m_bRandomHue;
};

inline Color3F Random(RandomColor rc)
{
	return rc.GetRandom();
}

template<>
struct TStorageTraits<Color3B>
{
	typedef Color3F				TValue;
	typedef RandomColor		TRandom;
};

template<>
struct TStorageTraits<Color3F>
{
	typedef Color3F				TValue;
	typedef RandomColor		TRandom;
};

///////////////////////////////////////////////////////////////////////
//
// Spline implementation class.
//

template<>
inline const CTypeInfo& TypeInfo(ISplineInterpolator**)
{
	static CTypeInfo Info("ISplineInterpolator*", sizeof(void*));
	return Info;
}

template<class S>
class TCurve: public spline::OptSpline< typename TStorageTraits<S>::TValue >
{
	typedef TCurve<S> TThis;
	typedef typename TStorageTraits<S>::TValue T;
	typedef spline::OptSpline<T> super_type;

	using_type(super_type, source_spline)
	using_type(super_type, key_type)
	using super_type::num_keys;
	using super_type::key;
	using super_type::empty;

public:
	// Implement serialization for particles.
	string ToString( FToString flags = 0 ) const;
	bool FromString( cstr str, FFromString flags = 0 );

	// Spline interface.
	ILINE void ModValue(T& val, float fTime) const
	{
		if (!empty())
		{
			T cval;
			interpolate(fTime, cval);
			val *= cval;
		}
	}
	T GetMinValue() const
	{
		if (!empty())
		{
			T val;
			min_value(val);
			return val;
		}
		else
			return T(1.f);
	}
	bool IsIdentity() const
	{
		return GetMinValue() == T(1.f);
	}

	struct CCustomInfo;
	CUSTOM_STRUCT_INFO(CCustomInfo)
};

//
// Composite parameter types, incorporating base value, randomness, and lifetime curves.
//

template<class S>
struct TVarParam
{
	typedef typename TStorageTraits<S>::TValue T;
	using_type(TStorageTraits<S>, TRandom)

	// Construction.
	TVarParam()
		: m_Base()
	{
	}

	TVarParam(const T& tBase)
		: m_Base(tBase)
	{
	}

	void Set(const T& tBase)
	{ 
		m_Base = tBase;
	}
	void Set(const T& tBase, const TRandom& tRan)
	{ 
		m_Base = tBase;
		m_VarRandom = tRan;
	}

	//
	// Value extraction.
	//
	ILINE operator bool() const
	{
		return m_Base != S(0);
	}
	ILINE bool operator !() const
	{
		return m_Base == S(0);
	}

	ILINE T GetMaxValue() const
	{
		return m_Base;
	}

	ILINE T GetMinValue() const
	{
		return m_Base - m_Base * float(m_VarRandom);
	}

	ILINE TRandom GetRandomRange() const
	{
		return m_VarRandom;
	}

	ILINE T GetVarMod() const
	{
		T val = T(1.f);
		if (m_VarRandom)
		{
			T ran(Random(m_VarRandom));
			ran *= val;
			val -= ran;
		}
		return val;
	}

	ILINE T GetVarValue() const
	{
		return GetVarMod() * T(m_Base);
	}

	ILINE T GetVarValue(CChaosKey key) const
	{
		T val = m_Base;
		if (!!m_VarRandom)
			val -= key.Jumble(this) * m_VarRandom * val;
		return val;
	}

	ILINE T GetVarValue(float fRandom) const
	{
		T val = m_Base;
		val *= (1.f - fRandom * m_VarRandom);
		return val;
	}

	ILINE T GetValueFromMod( T val ) const
	{
		return T(m_Base) * val;
	}

	AUTO_STRUCT_INFO

protected:

	S						m_Base;						// Base and maximum value.
	TRandom			m_VarRandom;			// Random variation, multiplicative.
};

///////////////////////////////////////////////////////////////////////

template<class S>
struct TVarEParam: TVarParam<S>
{
	typedef typename TStorageTraits<S>::TValue T;

	// Construction.
	ILINE TVarEParam()
	{
	}

	ILINE TVarEParam(const T& tBase)
		: TVarParam<S>(tBase)
	{
	}

	//
	// Value extraction.
	//

	T GetMinValue() const
	{
		T val = TVarParam<S>::GetMinValue();
		val *= m_VarEmitterStrength.GetMinValue();
		return val;
	}

	bool IsConstant() const
	{
		return m_VarEmitterStrength.IsIdentity();
	}

	ILINE T GetVarMod(float fEStrength) const
	{
		T val = T(1.f);
		if (m_VarRandom)
			val -= Random(m_VarRandom);
		m_VarEmitterStrength.ModValue(val, fEStrength);
		return val;
	}

	ILINE T GetVarValue(float fEStrength) const
	{
		return GetVarMod(fEStrength) * T(m_Base);
	}

	ILINE T GetVarValue(float fEStrength, CChaosKey keyRan) const
	{
		T val = m_Base;
		if (!!m_VarRandom)
			val -= keyRan.Jumble(this) * m_VarRandom * val;
		m_VarEmitterStrength.ModValue(val, fEStrength);
		return val;
	}

	ILINE T GetVarValue(float fEStrength, float fRandom) const
	{
		T val = m_Base;
		val *= (1.f - fRandom * m_VarRandom);
		m_VarEmitterStrength.ModValue(val, fEStrength);
		return val;
	}

	AUTO_STRUCT_INFO

protected:
	TCurve<S>		m_VarEmitterStrength;

	// Dependent name nonsense.
	using TVarParam<S>::m_Base;
	using TVarParam<S>::m_VarRandom;
};

///////////////////////////////////////////////////////////////////////
template<class S>
struct TVarEPParam: TVarEParam<S>
{
	typedef typename TStorageTraits<S>::TValue T;

	// Construction.
	TVarEPParam()
	{}

	TVarEPParam(const T& tBase)
		: TVarEParam<S>(tBase)
	{}

	//
	// Value extraction.
	//

	T GetMinValue() const
	{
		T val = TVarEParam<S>::GetMinValue();
		val *= m_VarParticleLife.GetMinValue();
		return val;
	}

	ILINE bool IsConstant() const
	{
		return m_VarParticleLife.IsIdentity();
	}

	ILINE T GetValueFromBase( T val, float fParticleAge ) const
	{
		m_VarParticleLife.ModValue(val, fParticleAge);
		return val;
	}

	ILINE T GetValueFromMod( T val, float fParticleAge ) const
	{
		m_VarParticleLife.ModValue(val, fParticleAge);
		return T(m_Base) * val;
	}

	using TVarParam<S>::GetValueFromMod;

	AUTO_STRUCT_INFO

protected:

	TCurve<S>	m_VarParticleLife;

	// Dependent name nonsense.
	using TVarEParam<S>::m_Base;
};


///////////////////////////////////////////////////////////////////////
// Special surface type enum

struct CSurfaceTypeIndex
{
	uint16	nIndex;

	STRUCT_INFO
};

///////////////////////////////////////////////////////////////////////
struct STextureTiling
{
	uint16	nTilesX, nTilesY;	// $<Min=1> $<Max=256> Number of tiles texture is split into 
	uint16	nFirstTile;				// First (or only) tile to use.
	uint16	nVariantCount;		// $<Min=1> Number of randomly selectable tiles; 0 or 1 if no variation.
	uint16	nAnimFramesCount;	// $<Min=1> Number of tiles (frames) of animation; 0 or 1 if no animation.
	TSmallBool bAnimCycle;		// Cycle animation, otherwise stop on last frame.
	TSmallBool bAnimBlend;		// Blend textures between frames.
	UFloat	fAnimFramerate;		// $<SoftMax=60> Tex framerate; 0 = 1 cycle / particle life.

	STextureTiling()
	{
		nFirstTile = 0;
		fAnimFramerate = 0.f;
		bAnimCycle = 0;
		bAnimBlend = 0;
		nTilesX = nTilesY = nVariantCount = nAnimFramesCount = 1;
	}

	int GetTileCount() const
	{
		return nTilesX * nTilesY - nFirstTile;
	}

	int GetFrameCount() const
	{
		return nVariantCount * nAnimFramesCount;
	}

	void Correct()
	{
		nTilesX = ::max(nTilesX, (uint16)1);
		nTilesY = ::max(nTilesY, (uint16)1);
		nFirstTile = ::min((uint16)nFirstTile, (uint16)(nTilesX*nTilesY-1));
		nAnimFramesCount = ::max(::min((uint16)nAnimFramesCount, (uint16)GetTileCount()), (uint16)1);
		nVariantCount = ::max(::min((uint16)nVariantCount, (uint16)(GetTileCount() / nAnimFramesCount) ), (uint16)1);
	}

	AUTO_STRUCT_INFO
};

///////////////////////////////////////////////////////////////////////
//! Particle system parameters
struct ParticleParams
{
	TVarEParam<UFloat> fCount;						// Number of particles in system at once.
	TVarParam<UFloat> fEmitterLifeTime;		// Min lifetime of the emitter, 0 if infinite. Always emits at least nCount particles.
	TVarParam<UFloat> fSpawnDelay;				// Delay the actual spawn time by this value	
	TVarParam<SFloat> fPulsePeriod;				// Time between auto-restarts of finite systems. 0 if just once, or continuous.

	// Particle timing
	TVarEParam<UFloat> fParticleLifeTime;	// Lifetime of particle
	TSmallBool bRemainWhileVisible;				// Particles will only die when not rendered (by any viewport).

	// Spawning
	TSmallBool bSecondGeneration;					// Emitters tied to each parent particle.
	TSmallBool bSpawnOnParentCollision;		// (Second Gen only) Spawns when parent particle collides.
	TSmall<EGeomType> eAttachType;				// Where to emit from attached geometry.
	TSmall<EGeomForm> eAttachForm;				// How to emit from attached geometry.
	Vec3S vPositionOffset;								// Spawn Position offset from the effect spawn position
	Vec3U vRandomOffset;									// Random offset of the particle relative position to the spawn position
	UFloat fPosRandomOffset;							// Radial random offset.

	// Angles
	TVarEParam<UHalfAngle> fFocusAngle;		// Angle to vary focus from default (Y axis), for variation.
	TVarEParam<SFloat> fFocusAzimuth;			// $<SoftMax=360> Angle to rotate focus about default, for variation. 0 = Z axis.
	TVarEParam<UHalfAngle> fEmitAngle;		// Angle from focus dir (emitter Y), in degrees. RandomVar determines min angle.
	
	// Sound
	CCryName sSound;											// Name of the sound file
	TVarEParam<UFloat> fSoundFXParam;			// Custom real-time sound modulation parameter.
	TSmall<ESoundControlTime> eSoundControlTime;		// The sound control time type
	//Angles (reordered for size)
	TSmallBool bFocusGravityDir;					// Uses negative gravity dir, rather than emitter Y, as focus dir.

	// Emitter params.
	TSmallBool bEnabled;									// Set false to disable this effect.
	TSmallBool bContinuous;								// Emit particles gradually until nCount reached

	// Appearance
	TSmall<EParticleFacing> eFacing;			// The basic particle shape type.
	TSmall<EParticleBlendType> eBlendType;// Blend rendering type.
	CCryName sTexture;										// Texture for particle.
	STextureTiling TextureTiling;					// Animation etc.	
	CCryName sMaterial;										// Material (overrides texture).
	CCryName sGeometry;										// Geometry for 3D particles.
	TVarEPParam<UFloat> fAlpha;						// Alpha value (opacity, or multiplier for additive).
	TVarEPParam<Color3B> cColor;					// Color modulation.
	UnitFloat fAlphaTest;									// Alpha test reference value (AlphaTest from material overrides this).
	TSmallBool bOrientToVelocity;					// Rotate particle X axis in velocity direction.
	TSmallBool bGeometryInPieces;					// Spawn geometry pieces separately.
	TSmallBool bSoftParticle;							// Enable soft particle processing in the shader.
	TSmallBool bSimpleParticle;						// Simple particle with no color and lighting
	TSmallBool bOctagonalShape;						// Use octogonal shape for particles instead of quad.

	// Lighting
	TSmallBool bReceiveShadows;						// Shadows will cast on these particles.
	TSmallBool bCastShadows;							// Particles will cast shadows (currently only geom particles).
	TSmallBool bGlobalIllumination;				// Enable global illumination in the shader.
	TSmallBool bDiffuseCubemap;						// Use nearest deferred cubemap for diffuse lighting
	UFloat fDiffuseLighting;							// $<SoftMax=1> Multiplier for particle dynamic lighting.
	UnitFloat8 fDiffuseBacklighting;			// Fraction of diffuse lighting applied in all directions.
	TFixed<uint8,MAX_HEATSCALE> fHeatScale;	// Multiplier to thermal vision.
	UFloat fEmissiveLighting;							// $<SoftMax=1> Multiplier for particle emissive lighting.
	SFloat fEmissiveHDRDynamic;						// $<SoftMin=-2> $<SoftMax=2> Power to apply to engine HDR multiplier for emissive lighting in HDR.

	struct SLightSource
	{
		TVarEPParam<UFloat> fRadius;				// $<SoftMax=10>
		TVarEPParam<UFloat> fIntensity;			// $<SoftMax=10>
		SFloat fHDRDynamic;									// $<SoftMin=-2> $<SoftMax=2>
		AUTO_STRUCT_INFO
	} LightSource;

	// Size
	TVarEPParam<UFloat> fSize;						// $<SoftMax=10> Particle size.
	struct SStretch: TVarEPParam<UFloat>
	{
		SFloat fOffsetRatio;								// $<SoftMin=-1> $<SoftMax=1> Move particle center this fraction in direction of stretch.
		AUTO_STRUCT_INFO
	} fStretch;														// $<SoftMax=1> Stretch particle into moving direction
	struct STailLength: TVarEPParam<UFloat>
	{
		uint8 nTailSteps;										// $<SoftMax=16> Number of tail segments
		AUTO_STRUCT_INFO
	} fTailLength;												// $<SoftMax=10> Length of tail in seconds
	UFloat fMinPixels;										// $<SoftMax=10> Augment true size with this many pixels.

	// Connection
	struct SConnection
	{
		TSmallBool bConnectParticles;				// Particles are drawn connected serially with a line.
		TSmallBool bConnectToOrigin;				// Newest particle connected to emitter origin.
		TSmallBool bFluidTexture;						// Texture assigned by lifetime, not per particle.
		UFloat fTextureFrequency;						// Number of mirrored texture wraps in line.

		SConnection() : 
			fTextureFrequency(1.f)
		{}
		AUTO_STRUCT_INFO
	};
	SConnection Connection;

	// Movement
	TVarEParam<SFloat> fSpeed;						// Initial speed of a particle.
	SFloat fInheritVelocity;							// $<SoftMin=0> $<SoftMax=1> Fraction of emitter's velocity to inherit.
	TVarEPParam<UFloat> fAirResistance;		// $<SoftMax=10> Air drag value, in inverse seconds.
	UFloat fRotationalDragScale;					// $<SoftMax=1> Multipler to AirResistance, for rotational motion.
	TVarEPParam<SFloat> fGravityScale;		// $<SoftMin=-1> $<SoftMax=1> Multiplier for world gravity.
	Vec3S vAcceleration;									// Specific world-space acceleration vector.
	TVarEPParam<UFloat> fTurbulence3DSpeed;// $<SoftMax=10> 3D random turbulence force
	TVarEPParam<UFloat> fTurbulenceSize;	// $<SoftMax=10> Radius of turbulence
	TVarEPParam<SFloat> fTurbulenceSpeed;	// $<SoftMin=-360> $<SoftMax=360> Speed of rotation

	struct STargetAttraction							// Customise movement toward ParticleTargets.
	{
		TSmallBool bIgnore;
		TSmallBool bExtendSpeed;						// Extend particle speed as necessary to reach target in normal lifetime.
		TSmallBool bShrink;									// Shrink particle as it approaches target.
		TSmallBool bOrbit;
		TVarEPParam<SFloat> fOrbitDistance;	// If > 0, orbits at specified distance rather than disappearing.

		AUTO_STRUCT_INFO
	} TargetAttraction;

	// Rotation
	Vec3_Zero<SAngle> vInitAngles;				// Initial rotation in symmetric angles (degrees).
	Vec3_Zero<UFullAngle> vRandomAngles;	// Bidirectional random angle variation.
	Vec3S vRotationRate;									// $<SoftMin=-360> $<SoftMax=360> Rotation speed (degree/sec).
	Vec3U vRandomRotationRate;						// $<SoftMax=360> Random variation.

	// Collision
	TSmall<EParticlePhysicsType> ePhysicsType;
	TSmallBool bCollideTerrain;						// Collides with terrain (if Physics <> none)
	TSmallBool bCollideStaticObjects;			// Collides with static physics objects (if Physics <> none)
	TSmallBool bCollideDynamicObjects;		// Collides with dynamic physics objects (if Physics <> none)
	CSurfaceTypeIndex sSurfaceType;				// Surface type for physicalised particles.
	SFloat fBounciness;										// $<SoftMin=0> $<SoftMax=1> Elasticity: 0 = no bounce, 1 = full bounce, <0 = die.
	UFloat fDynamicFriction;							// $<SoftMax=10> Sliding drag value, in inverse seconds.
	UFloat fThickness;										// $<SoftMax=1> Lying thickness ratio - for physicalized particles only
	UFloat fDensity;											// $<SoftMax=2000> Mass density for physicslized particles.
	uint8 nMaxCollisionEvents;						// Max # collision events per particle (0 = no limit).

	// Visibility
	TSmallBool bBindEmitterToCamera;
	TSmallBool bMoveRelEmitter;						// Particle motion is in emitter space.
	TSmallBool bSpaceLoop;								// Lock particles in box around emitter position, use vSpaceLoopBoxSize to set box size
	UFloat fCameraMaxDistance;						// $<SoftMax=100> Max distance from camera to render particles.
	UFloat fCameraMinDistance;						// $<SoftMax=100> Min distance from camera to render particles.
	UFloat fViewDistanceAdjust;						// $<SoftMax=1> Multiplier to automatic distance fade-out.
	int8 nDrawLast;												// $<SoftMin=-16> $<SoftMax=16> Adjust draw order within effect group.
	TSmall<ETrinary> tVisibleIndoors;			// Whether visible indoors/outdoors/both.
	TSmall<ETrinary> tVisibleUnderwater;	// Whether visible under/above water / both.

	// Advanced
	TSmall<EParticleForceType> eForceGeneration;	// Generate physical forces if set.
	UFloat fFillRateCost;									// $<SoftMax=10> Adjustment to max screen fill allowed per emitter.
	UFloat fMotionBlurScale;							// $<SoftMax=2> Multiplier to motion blur.
	UFloat fMotionBlurStretchScale;				// $<SoftMax=10> Multiplier for motion blur sprite stretching based on particle movement.
	UFloat fMotionBlurCamStretchScale;		// $<SoftMax=10> Multiplier for motion blur sprite stretching based on camera movement.
	TSmallBool bNotAffectedByFog;
	TSmallBool bDrawNear;									// Render particle in near space (weapon)
	TSmallBool bDrawOnTop;								// Render particle on top of everything (no depth test)
	TSmallBool bNoOffset;									// Disable centering of static objects
	TSmallBool bEncodeVelocity;						// Orient and encode velocity direction, for special shaders.
	TSmallBool bAllowMerging;							// Do not merge drawcalls for particle emitters of this type
	TSmallBool bStreamable;								// True if the texture/geometry allowed to be streamed.
	TSmall<EParticleHalfResFlag> eAllowHalfRes;	// Do not use half resolution rendering

	// Configurations
	TSmall<EConfigSpecBrief> eConfigMin;
	TSmall<EConfigSpecBrief> eConfigMax;
	TSmall<ETrinary> tDX11;

	ParticleParams() 
	{
		memset(this, 0, sizeof(*this));
		new(this) ParticleParams(true);
	}

	AUTO_STRUCT_INFO

protected:

	ParticleParams(bool) :
		bEnabled(true),
		fSize(1.f),
		cColor(1.f),
		fAlpha(1.f),
		eBlendType(ParticleBlendType_AlphaBased),
		fDiffuseLighting(1.f),
		bStreamable(true),
		bOctagonalShape(true),
		fViewDistanceAdjust(1.f),
		fFillRateCost(1.f),
		fRotationalDragScale(1.f),
		fDensity(1000.f),
		fThickness(1.f),
		fMotionBlurStretchScale(1.f),
		fMotionBlurCamStretchScale(1.f),
		fSoundFXParam(1.f),
		bAllowMerging(true),
		eConfigMax(ConfigSpec_VeryHigh),
		eSoundControlTime(SoundControlTime_EmitterLifeTime)
	{}
};

#endif
