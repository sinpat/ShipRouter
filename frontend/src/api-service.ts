import Axios from 'axios';
import { ICoordinate, IGridNode, Path } from './types';

const endpoint = 'http://localhost:9090/';
// const endpoint = 'http://backend:9090';

class ApiService {
  public async snapNode(latlng: ICoordinate) {
    return Axios.get<IGridNode>(endpoint + 'snap', {
      params: latlng,
    }).then(({ data }) => data);
  }

  public async shortestPath(source: number, target: number): Promise<Path> {
    return Axios.get<{
      lats: number[];
      lngs: number[];
      distance: number;
    }>(endpoint + 'route', {
      params: {
        source,
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
