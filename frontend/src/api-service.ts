import Axios from 'axios';
import { Coordinate, Path } from './types';

const endpoint = 'http://localhost:8000/';

class ApiService {
  public async snapNode(latlng: Coordinate) {
    return Axios.get<Coordinate>(endpoint + 'snap', {
      params: latlng,
    }).then(({ data }) => data);
  }

  public async shortestPath(
    start: Coordinate,
    target: Coordinate
  ): Promise<Path> {
    return Axios.post<{
      lats: number[];
      lngs: number[];
      distance: number;
    }>(endpoint + 'route', null, {
      params: {
        start,
        target,
      },
    }).then(({ data }) => {
      const { lats, lngs, distance } = data;
      if (!lats.length || !lngs.length) {
        throw new Error('Could not find shortest path');
      }
      return {
        coordinates: lats.map((lat, idx) => ({
          lat,
          lng: lngs[idx],
        })),
        distance,
      };
    });
  }
}

export default new ApiService();
