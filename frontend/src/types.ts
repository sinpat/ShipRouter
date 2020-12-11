export type Coordinate = {
  lat: number;
  lng: number;
};

export type Path = {
  coordinates: Coordinate[];
  distance: number;
};
