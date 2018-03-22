////////////////////////////////////////////////////////////////////////
/// \file  larcorealg/Geometry/WireGeo.h
/// \brief Encapsulate the geometry of a wire
/// \ingroup Geometry
///
/// \author  brebel@fnal.gov
////////////////////////////////////////////////////////////////////////

#ifndef LARCOREALG_GEOMETRY_WIREGEO_H
#define LARCOREALG_GEOMETRY_WIREGEO_H

// LArSoft
#include "larcorealg/Geometry/LocalTransformationGeo.h"
#include "larcorealg/Geometry/geo_vectors_utils.h" // geo::vect

// ROOT
#include "TVector3.h"
#include "TGeoMatrix.h" // TGeoHMatrix

// C/C++ libraries
#include <vector>
#include <cmath> // std::sin(), ...


// forward declarations
class TGeoNode;


namespace geo {
  
  struct WireID; // forward declaration
  
  
  /** **************************************************************************
   * @brief Geometry description of a TPC wire
   * @ingroup Geometry
   * 
   * The wire is a single straight segment on a wire plane.
   * Different wires may be connected to the same readout channel. That is of
   * no relevance for the geometry description.
   * 
   * The wire has a start and an end point. Their definition of them is related
   * to the other wires in the plane and to the TPC itself.
   * 
   * The direction of increasing wire coordinate, defined in the wire plane,
   * is orthogonal to the wire direction and of course points to the direction
   * where the wire number within the plane increases. This direction is
   * indirectly defined when sorting the wires in the plane, which is done by
   * the plane (geo::PlaneGeo). This direction lies by definition on the wire
   * plane. The direction normal to the wire plane is defined by the TPC so that
   * it points inward the TPC rather than outward.
   * Finally, the wire direction is defined so that the triplet of unit vectors
   * direction of the wire @f$ \hat{l} @f$, direction of increasing wire number
   * @f$ \hat{w} @f$, and normal to the plane @f$ \hat{n} @f$ is positively
   * defined (@f$ \hat{l} \times \hat{w} \cdot \hat{n} = +1 @f$).
   * The start @f$ \vec{a}_{w} @f$ and the end of the wire @f$ \vec{b}_{w} @f$
   * are defined so that their difference @f$ \vec{b}_{w} - \vec{a}_{w} @f$
   * points in the same direction as @f$ \hat{l} @f$.
   * 
   */
  class WireGeo {
    
    using DefaultVector_t = TVector3; // ... not for long
    using DefaultPoint_t = TVector3; // ... not for long
    
  public:
    
    using GeoNodePath_t = std::vector<TGeoNode const*>;
    
    /// @{
    /**
     * @name Types for geometry-local reference vectors.
     * 
     * These types represents points and displacement vectors in the reference
     * frame defined in the wire geometry "box" from the GDML geometry
     * description.
     * 
     * No alias is explicitly defined for the LArSoft global vector types,
     * `geo::Point_t` and `geo::Vector_t`.
     * 
     * Remember the `LocalPoint_t` and `LocalVector_t` vectors from different
     * instances of `geo::WireGeo` have the same type but are not compatible.
     */
    
    /// Tag for vectors in the "local" GDML coordinate frame of the plane.
    struct WireGeoCoordinatesTag {};
    
    /// Type of points in the local GDML wire plane frame.
    using LocalPoint_t = geo::Point3DBase_t<WireGeoCoordinatesTag>;
    
    /// Type of displacement vectors in the local GDML wire plane frame.
    using LocalVector_t = geo::Vector3DBase_t<WireGeoCoordinatesTag>;
    
    ///@}
    
    WireGeo(GeoNodePath_t const& path, size_t depth);
    
    
    /// @{
    /// @name Size and coordinates
    
    /// Returns the outer half-size of the wire [cm]
    double RMax() const;
    
    /// Returns half the length of the wire [cm]
    double HalfL() const { return fHalfL; }
    
    /// Returns the inner radius of the wire (usually 0) [cm]
    double RMin() const;
    
    /**
     * @brief Fills the world coordinate of a point on the wire
     * @param xyz _(output)_ the position to be filled, as [ x, y, z ] (in cm)
     * @param localz distance of the requested point from the middle of the wire
     * @see `GetCenter()`, `GetStart()`, `GetEnd()`, `GetPositionFromCenter()`
     * 
     * The center of the wires corresponds to `localz` equal to `0`; negative
     * positions head toward the start of the wire, positive toward the end.
     * 
     * If the `localz` position would put the point outside the wire, the
     * returned position is the wire end closest to the requested position.
     * 
     * @deprecated Use the version returning a vector instead.
     */
    void GetCenter(double* xyz, double localz=0.0) const;
    
    /// Fills the world coordinate of one end of the wire
    /// @deprecated Use the version returning a vector instead.
    void GetStart(double* xyz) const { GetCenter(xyz, -fHalfL); }
    
    /// Fills the world coordinate of one end of the wire
    /// @deprecated Use the version returning a vector instead.
    void GetEnd(double* xyz) const { GetCenter(xyz, +fHalfL); }

    /**
     * @brief Returns the position (world coordinate) of a point on the wire
     * @tparam Point type of vector to be returned (current default: `TVector3`)
     * @param localz distance of the requested point from the middle of the wire
     * @return the position of the requested point (in cm)
     * @see `GetCenter()`, `GetStart()`, `GetEnd()`,
     *      `GetPositionFromCenterUnbounded()`
     * 
     * The center of the wires corresponds to `localz` equal to `0`; negative
     * positions head toward the start of the wire, positive toward the end.
     * 
     * If the `localz` position would put the point outside the wire, the
     * returned position is the wire end closest to the requested position.
     */
    template <typename Point = DefaultPoint_t>
    Point GetPositionFromCenter(double localz) const
      { return GetPositionFromCenterUnbounded<Point>(capLength(localz)); }
    
    /**
     * @brief Returns the position (world coordinate) of a point on the wire
     * @tparam Point type of vector to be returned (current default: `TVector3`)
     * @param localz distance of the requested point from the middle of the wire
     * @return the position of the requested point (in cm)
     * @see `GetCenter()`, `GetStart()`, `GetEnd()`, `GetPositionFromCenter()`
     * 
     * The center of the wires corresponds to `localz` equal to `0`; negative
     * positions head toward the start of the wire, positive toward the end.
     * 
     * If the `localz` position would put the point outside the wire, the
     * returned position will lie beyond the end of the wire.
     */
    template <typename Point = DefaultPoint_t>
    Point GetPositionFromCenterUnbounded(double localz) const;
    
    /// Returns the world coordinate of the center of the wire [cm]
    template <typename Point = DefaultPoint_t>
    Point GetCenter() const { return geo::vect::convertTo<Point>(fCenter); }
    
    /// Returns the world coordinate of one end of the wire [cm]
    template <typename Point = DefaultPoint_t>
    Point GetStart() const
      { return GetPositionFromCenterUnbounded<Point>(-HalfL()); }
    
    /// Returns the world coordinate of one end of the wire [cm]
    template <typename Point = DefaultPoint_t>
    Point GetEnd() const
      { return GetPositionFromCenterUnbounded<Point>(+HalfL()); }
    
    /// Returns the wire length in centimeters
    double Length() const { return 2. * HalfL(); }
    
    /// @}
    
    /// @{
    /// @name Orientation and angles
    
    /// Returns angle of wire with respect to z axis in the Y-Z plane in radians
    double ThetaZ() const { return fThetaZ; }
    
    /**
     * Returns angle of wire with respect to z axis in the Y-Z plane
     * @param degrees return the angle in degrees rather than radians
     * @return wire angle
     */
    double ThetaZ(bool degrees) const;
    
    //@{
    /// Returns trigonometric operations on ThetaZ()
    double CosThetaZ() const { return std::cos(ThetaZ()); }
    double SinThetaZ() const { return std::sin(ThetaZ()); }
    double TanThetaZ() const { return std::tan(ThetaZ()); }
    //@}
    
    /// Returns if this wire is horizontal (theta_z ~ 0)
    bool isHorizontal() const { return std::abs(SinThetaZ()) < 1e-5; }
    
    /// Returns if this wire is vertical (theta_z ~ pi/2)
    bool isVertical() const { return std::abs(CosThetaZ()) < 1e-5; }
    
    /// Returns if this wire is parallel to another
    bool isParallelTo(geo::WireGeo const& wire) const
      {
        return // parallel if the dot product of the directions is about +/- 1
          std::abs(std::abs(Direction<geo::Vector_t>().Dot(wire.Direction<geo::Vector_t>())) - 1.) < 1e-5;
      }
    
    /// Returns the wire direction as a norm-one vector
    template <typename Vector = DefaultVector_t>
    Vector Direction() const;
    
    /**
     * @brief Prints information about this wire.
     * @tparam Stream type of output stream to use
     * @param out stream to send the information to
     * @param indent prepend each line with this string
     * @param verbosity amount of information printed
     * 
     * Note that the first line out the output is _not_ indented.
     * 
     * Verbosity levels
     * -----------------
     * 
     * * 0: only start and end
     * * 1 _(default)_: also length
     * * 2: also angle with z axis
     * * 3: also center
     * * 4: also direction
     * 
     * The constant `MaxVerbosity` is set to the highest supported verbosity
     * level.
     */
    template <typename Stream>
    void PrintWireInfo
      (Stream&& out, std::string indent = "", unsigned int verbosity = 1) const;
    
    /// Maximum verbosity supported by `PrintWireInfo()`.
    static constexpr unsigned int MaxVerbosity = 4;
    
    /// @}
    
    
    /// @{
    /// @name Coordinate conversion
    
    /// @{
    /**
     * @name Coordinate transformation
     * 
     * Local points and displacement vectors are described by the types
     * `geo::WireGeo::LocalPoint_t` and `geo::WireGeo::LocalVector_t`,
     * respectively.
     */
    
    /// Transform point from local wire frame to world frame.
    void LocalToWorld(const double* wire, double* world) const
      { fTrans.LocalToWorld(wire, world); }
      
    /// Transform point from local wire frame to world frame.
    geo::Point_t toWorldCoords(LocalPoint_t const& local) const
      { return fTrans.toWorldCoords(local); }
    
    /// Transform direction vector from local to world.
    void LocalToWorldVect(const double* wire, double* world) const
      { fTrans.LocalToWorldVect(wire, world); }
    
    /// Transform direction vector from local to world.
    geo::Vector_t toWorldCoords(LocalVector_t const& local) const
      { return fTrans.toWorldCoords(local); }
    
    /// Transform point from world frame to local wire frame.
    void WorldToLocal(const double* world, double* wire) const
      { fTrans.WorldToLocal(world, wire); }
    
    /// Transform point from world frame to local wire frame.
    LocalPoint_t toLocalCoords(geo::Point_t const& world) const
      { return fTrans.toLocalCoords(world); }
    
    /// Transform direction vector from world to local.
    void WorldToLocalVect(const double* world, double* wire) const
      { fTrans.WorldToLocalVect(world, wire); }
    
    /// Transform direction vector from world to local.
    LocalVector_t toLocalCoords(geo::Vector_t const& world) const
      { return fTrans.toLocalCoords(world); }
    
    /// @}
    
    
    const TGeoNode*     Node() const { return fWireNode; }
    
    /// Returns the z coordinate, in centimetres, at the point where y = 0.
    /// Assumes the wire orthogonal to x axis and the wire not parallel to z.
    double ComputeZatY0() const
      { return fCenter.Z() - fCenter.Y() / TanThetaZ(); }
    
    /**
     * @brief Returns 3D distance from the specified wire
     * @return the signed distance in centimetres (0 if wires are not parallel)
     * 
     * If the specified wire is "ahead" in z respect to this, the distance is
     * returned negative.
     */
    double DistanceFrom(geo::WireGeo const& wire) const;
    
    
    /// Internal updates after the relative position of the wire is known
    /// (currently no-op)
    void UpdateAfterSorting(geo::WireID const&, bool flip);
    
    /// Returns the pitch (distance on y/z plane) between two wires, in cm
    static double WirePitch(geo::WireGeo const& w1, geo::WireGeo const& w2)
      { return std::abs(w2.DistanceFrom(w1)); }
    
  private:
    using LocalTransformation_t
      = geo::LocalTransformationGeo<TGeoHMatrix, LocalPoint_t, LocalVector_t>;
    
    const TGeoNode*    fWireNode;  ///< Pointer to the wire node
    double             fThetaZ;    ///< angle of the wire with respect to the z direction
    double             fHalfL;     ///< half length of the wire
    geo::Point_t       fCenter;    ///< Center of the wire in world coordinates.
    LocalTransformation_t fTrans;  ///< Wire to world transform.
    bool               flipped;    ///< whether start and end are reversed
    
    /// Returns whether ( 0, 0, fHalfL ) identifies end (false) or start (true)
    /// of the wire.
    bool isFlipped() const { return flipped; }
    
    /// Returns the relative length from center to be used when transforming.
    double relLength(double local) const { return isFlipped()? -local: local; }
    
    /// Caps the specified local length coordinate to lay on the wire.
    double capLength(double local) const
      { return std::min(+HalfL(), std::max(-HalfL(), local)); }
    
    /// Stacked `capLength()` and `relLength()`.
    double capRelLength(double local) const
      { return capLength(relLength(local)); }
    
    /// Set to swap the start and end wire
    void Flip();
    
    
    static double gausSum(double a, double b) { return std::sqrt(a*a + b*b); }
    
  };
}


//------------------------------------------------------------------------------
//--- template implementation
//---
//------------------------------------------------------------------------------
template <typename Point>
Point geo::WireGeo::GetPositionFromCenterUnbounded(double localz) const {
  return geo::vect::convertTo<Point>
    (toWorldCoords(LocalPoint_t{ 0.0, 0.0, relLength(localz) }));
} // geo::WireGeo::GetPositionFromCenterImpl()


//------------------------------------------------------------------------------
template <typename Vector /* = DefaultVector_t */>
Vector geo::WireGeo::Direction() const {
  // maybe (GetCenter() - GetStart()) / HalfL() would be faster;
  // strangely, TVector3 does not implement operator/ (double).
  return geo::vect::convertTo<Vector>(GetEnd<geo::Point_t>() - GetStart<geo::Point_t>()) * (1.0 / Length());
} // geo::WireGeo::Direction()


//------------------------------------------------------------------------------
template <typename Stream>
void geo::WireGeo::PrintWireInfo(
  Stream&& out,
  std::string indent /* = "" */,
  unsigned int verbosity /* = 1 */
) const {

  //----------------------------------------------------------------------------
  out << "wire from " << GetStart<geo::Point_t>()
    << " to " << GetEnd<geo::Point_t>();
  
  if (verbosity-- <= 0) return; // 0
  
  //----------------------------------------------------------------------------
  out << " (" << Length() << " cm long)";
  
  if (verbosity-- <= 0) return; // 1
  
  //----------------------------------------------------------------------------
  out << ", theta(z)=" << ThetaZ() << " rad";
  
  if (verbosity-- <= 0) return; // 2
  
  //----------------------------------------------------------------------------
  out << "\n" << indent
    << "  center at " << GetCenter<geo::Point_t>() << " cm";
    
  if (verbosity-- <= 0) return; // 3
  
  //----------------------------------------------------------------------------
  out << ", direction: " << Direction<geo::Vector_t>();
  if (isHorizontal()) out << " (horizontal)";
  if (isVertical()) out << " (vertical)";
    
//  if (verbosity-- <= 0) return; // 4
  
  //----------------------------------------------------------------------------
} // geo::WireGeo::PrintWireInfo()


//------------------------------------------------------------------------------

#endif // LARCOREALG_GEOMETRY_WIREGEO_H
