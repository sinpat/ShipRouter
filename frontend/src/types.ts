export interface ICoordinate {
  lat: number;
  lng: number;
}

export interface IGridNode extends ICoordinate {
  id: number;
}

export type Path = {
  coordinates: ICoordinate[];
  distance: number;
};
