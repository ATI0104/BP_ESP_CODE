// author: xkovacs60
import { useConfigStore } from '@/stores/configStore'
import axios, { type CreateAxiosDefaults } from 'axios'

export default () => {
  const configStore = useConfigStore()
  const options: CreateAxiosDefaults = { baseURL: configStore.API_URL }
  const instance = axios.create(options)

  instance.interceptors.response.use(
    (response) => {
      return response
    },
    (error) => {
      return Promise.reject(error)
    }
  )
  return instance
}
